//
// Created by Michal on 08/02/16.
//

#include "Middleware.h"

Middleware::Middleware() {
    this->fps = FPS; // overwritten from bpm window
    this->initialFlag = false;
    this->working = false;
    this->bpm = 0;
    this->bpmDetected = false;
};

/**
 * Initialize compute
 */
void Middleware::compute(vector<Mat> videoBuffer){
    // Just to be extra sure worker is ready
    if (this->working) return;

    // Start work!
    this->working = true;
    this->bpmDetected = false;
    cout << "Computing bpm";

    // At first fill class buffer with copies!
    this->setVideoBuffer(videoBuffer);

    // Max Level for masking
    int level = 6;

    // Resizing should be set according to face SIZE !!
    BpmVideoProcessor bpmVideoProcessor = BpmVideoProcessor(videoBuffer, CUTOFF_FL, CUTOFF_FH, level, fps, bufferFrames, faceRoi);

    // Compute bpm at first
    bpmVideoProcessor.computeBpm();
    updateBpm(bpmVideoProcessor.getBpm());

    // Magic inside class
    bpmVideoProcessor.computeAmplifiedMask();
    this->visualization = bpmVideoProcessor.getOut();

    // Resize and crop video to similar size as face in gui
    resizeCropVideo(this->visualization, this->resizedFace.width);

    // Cleaning
    this->videoBuffer.clear();
    this->working = false;
    this->initialFlag = true;

};

/**
 * Updates current bpm with respect to previous value
 * prevents big changes
 */
void Middleware::updateBpm(int bpm) {
    // Prevent big changes
    this->bpm = this->bpm ? (this->bpm + bpm) / 2 : bpm;
    this->bpmDetected = true;
}
