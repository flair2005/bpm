//
// Created by Michal on 26/03/16.
//

#ifndef BPM_BPMVIDEOPROCESSOR_H
#define BPM_BPMVIDEOPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/core.hpp>

#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include "imageOperation.h"


// Configuration
#include "./../config.h"

class BpmVideoProcessor {
    // Input face video - should be const
    vector<Mat> faceVideo;
    vector<Mat> origVideo;

    // Try to extract more sking to this
    vector<Mat> forehead;

    vector<Mat> out; // Blood circulation visualization
    vector<Mat> temporalSpatial; // Temporal spatial

    vector <vector <Mat> > pyramid;
    vector <vector <Mat> > pyramidForehead;

    Rect foreheadRoi;
    Rect faceRoi;

    vector<double> foreheadIntensities; // Intensities of forehead area

    // Computed bpm
    int bpm;

    // Lower & Higher cutoff
    float fl, fh;

    // FPS of input
    int fps;

    // Number of frames in face buffer
    int framesCount;

    // Level of double decreasing size of each frame
    // For mask computation
    int level;

    // How wide freq (bpm) range should we keep
    int maskWidth;

    public:
        BpmVideoProcessor(vector<Mat> video, float fl, float fh, int level, int fps, int framesCount, Rect faceRoi);
        void computeAmplifiedMask();
        void buildGDownPyramid(vector<Mat> &src, vector<vector <Mat> > &pyramid, int level);
        void amplifyFrequencyInPyramid(vector<vector <Mat> > &pyramid, vector<Mat> &temporalSpatial, vector<Mat> &dst, float bpm);
        void reconstructMaskFromPyramid (vector<vector<Mat> > &pyramid, vector <Mat> & dst);
        void getForeheadSkinArea();

        void computeBpm(int computeType = AVG_COMPUTE);
        void computeBpmFromPyramid();

        // Static functions
        static void amplifyVideoChannels(vector<Mat>& video, float r, float g, float b);

        static void buildGDownPyramidLevel(vector<Mat> &src, vector<Mat> &dst, int currLevel) {
            for (int i = 0; i < src.size(); i++) {
                // 0 Level only copy
                if (currLevel == 0) {
                    src[i].convertTo(src[i], CV_32FC3);
                    dst.push_back(src[i]);
                    continue;
                }

                if (src[i].type() == CV_32FC3) {
                    src[i].convertTo(src[i], CV_8UC3);
                }

                cvtColor2(src[i], src[i], CV2_BGR2YIQ); // returns CV_8UC3
                src[i].convertTo(src[i], CV_32FC3);

                pyrDown(src[i], src[i]);

                src[i].convertTo(src[i], CV_8UC3);
                cvtColor2(src[i], src[i], CV2_YIQ2BGR); // returns CV_8UC3
                src[i].convertTo(src[i], CV_32FC3);
                dst.push_back(src[i]);
            }
        }

        static void amplifyFrequencyInLevel(vector<Mat> src, vector<Mat> &temporalSpatial, vector<Mat> &dst, float bpm, int fps) {
            // Create temporal spatial video
            createTemporalSpatial(src, temporalSpatial);

            // Bandpass temporal video
            bandpassFilter(temporalSpatial, bpm, fps);

            // Inverse temporal spatial to video
            inverseTemporalSpatial(temporalSpatial, dst);
        }

        static void createTemporalSpatial(vector<Mat> src, vector<Mat>& temporalSpatial) {
            int dstVectorCount = src[0].size().width;
            int dstVectorWidth = (int) src.size();
            int dstVectorHeight = src[0].size().height;

            for (int i = 0; i < dstVectorCount; i++) {
                // One frame
                Mat frame(dstVectorHeight, dstVectorWidth, CV_32FC1);
                for (int j = 0; j < dstVectorWidth; j++) {
                    Mat tmp = src[j].clone();
                    cvtColor(tmp, tmp, CV_BGR2GRAY);
                    tmp.convertTo(tmp, CV_32F);

                    for(int k = 0; k < dstVectorHeight; k++) {
                        frame.at<float>(k,j) = tmp.at<float>(k,i);
                    }
                }
                temporalSpatial.push_back(frame.clone());
                frame.release();
            }
        }

        static void inverseTemporalSpatial(vector<Mat>& temporalSpatial, vector<Mat>& dst) {
            int dstCount = temporalSpatial[0].cols;
            int dstWidth = (int) temporalSpatial.size();
            int dstHeight = temporalSpatial[0].rows;

            for (int i = 0; i < dstCount; i++) {
                Mat outputFrame = Mat(dstHeight, dstWidth, CV_32FC1);

                for (int j = 0; j < dstWidth; j++) {
                    for(int k = 0; k < dstHeight; k++) {
                            outputFrame.at<float>(k, j) = temporalSpatial[j].at<float>(k, i);
                    }
                }

                cvtColor(outputFrame, outputFrame, CV_GRAY2BGR);
                outputFrame.convertTo(outputFrame, CV_8U);

                // Keep only red channel
                // TODO: Do weights make sense?
                amplifyChannels(outputFrame, 1.0, .5f, 0.1f);

                // in range [0,255]
                normalize(outputFrame, outputFrame, 0, 150, NORM_MINMAX );

                dst.push_back(outputFrame);
                outputFrame.release();
            }
        }

        // TODO: Check - may not work properly
        static void bandpassFilter(vector<Mat> &temporalSpatial, float freq, int fps) {

            // Create mask based on strongest frequency

            int width = cv::getOptimalDFTSize(temporalSpatial[0].cols);
            int height = cv::getOptimalDFTSize(temporalSpatial[0].rows);

            Mat row = generateFreqMask(freq, width, fps);
            Mat filter(height, width, CV_32FC1);

            for (int i = 0; i < filter.rows; i++) {
                row.copyTo(filter.row(i));
            }

            for (int i = 0; i < temporalSpatial.size(); i++) {

                Mat frame = temporalSpatial[i];
                Mat tmp;

                cv::copyMakeBorder(frame, tmp,
                                   0, height - frame.rows,
                                   0, width - frame.cols,
                                   cv::BORDER_CONSTANT, cv::Scalar::all(0));
                // apply DFT
                cv::dft(tmp, tmp, cv::DFT_ROWS | cv::DFT_SCALE);

                // Filter
                cv::mulSpectrums(tmp, filter, tmp, cv::DFT_ROWS);

                // apply inverse DFT
                cv::idft(tmp, tmp, cv::DFT_ROWS | cv::DFT_SCALE);

                // Copy with roi to temporal spatial frame
                tmp(cv::Rect(0, 0, frame.cols, frame.rows)).copyTo(temporalSpatial[i]);

                normalize(temporalSpatial[i], temporalSpatial[i], 0, 1, CV_MINMAX);

            }

        }


        //  Getters & setters
        const vector<Mat> &getOut() const {
            return out;
        }
        const vector<Mat> &getTemporalSpatialMask() const {
            return temporalSpatial;
        }
        int getBpm() const {
            return bpm;
        }
};

class PyramidLevelWorker {
    vector<Mat> result;

    public:

        void computeGDownPyramidLevel(vector<Mat> &src, int currLevel) {
            BpmVideoProcessor::buildGDownPyramidLevel(src, result, currLevel);
            int i = 0;
        }

        void computeAmplificationPyramidLevel (vector<Mat> src, float bpm, int fps) {
            vector<Mat> temporalSpatial;
            BpmVideoProcessor::amplifyFrequencyInLevel(src, temporalSpatial, result, bpm, fps);
            temporalSpatial.clear();
        }

        void reconstructPyramidLevel(vector<vector<Mat> > &pyramid, int currLevel) {
            pyrUpVideo(pyramid.at(currLevel), result, pyramid.at(0)[0].size(), currLevel);
        }

        vector<Mat> &getResult() {
            return result;
        }
};

#endif //BPM_BPMVIDEOPROCESSOR_H