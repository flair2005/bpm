//
// Created by Michal on 04/03/16.
//

#include "Detector.h"

Detector::Detector() {
    this->faceHeightScale = 1.3f;
    this->faceYOffset = 0.1f;
    this->working = false;
    this->faceCascade.load((string) DATA_DIR+"/haarcascades/haarcascade_frontalface_alt.xml");
}

void Detector::detectFace(Mat frame) {
    if (this->working) return;

    // Start work!
    this->working = true;
    this->frame = frame;

    // Create bw mat
    cvtColor(frame, frame, CV_BGR2GRAY );

    // Detect faces
    faceCascade.detectMultiScale( frame, faces, 1.3, 2, 0|CV_HAAR_SCALE_IMAGE);

    // Determine biggest face
    this->biggestFace = this->determineBiggestFace();

    // Check whether detection successful
    if (!this->faces.size()) {
        this->working = false;
        return;
    }

    this->adjustFaceSize();
    this->working = false;
}

// Increase height of detected face
void Detector::adjustFaceSize() {
    this->biggestFace.y -= this->biggestFace.height*faceYOffset;
    this->biggestFace.height *= faceHeightScale;

    // Control range
    handleRoiPlacement(this->biggestFace, this->frame.size());
}



Rect &Detector::determineBiggestFace() {
    int biggestFaceIndex = 0;
    int biggestFaceArea = 0;
    for (int i = 0; i < faces.size(); i++) {
        if (faces[i].area() > biggestFaceArea) {
            biggestFaceIndex = i;
            biggestFaceArea = faces[i].area();
        }
    }
    return this->faces[biggestFaceIndex];
}

Rect &Detector::getMainFace(Mat frame) {
    Point2d centerFrame = getCenter(frame.size());

    int closestFaceIndex = 0;
    double minDistance = getDistance(centerFrame, getCenter(faces[0].size()));

    for (int i = 1; i < faces.size(); i++) {
        if (getDistance(centerFrame, getCenter(faces[i].size())) < minDistance) {
            closestFaceIndex = 0;
            minDistance = getDistance(centerFrame, getCenter(faces[i].size()));
        }
    }
    return this->faces[closestFaceIndex];
}

int detectEyes(Mat face, vector<Rect>& eyes) {
    vector<Rect> eyesAll;

    CascadeClassifier eyesCascade;
    eyesCascade.load((string) DATA_DIR+"/haarcascades/haarcascades_eye.xml");
    eyesCascade.detectMultiScale(face, eyesAll, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(20,20));

    // We need 2 eyes
    if (eyesAll.size() < 2) {
        return 0;
    }

    // Return 2 eyes with closest y diff
    int eyeFirstIndex = 0;
    int eyeSecondIndex = 1;
    float yDiff = abs(eyesAll[eyeFirstIndex].y - eyesAll[eyeSecondIndex].y);

    for (int i = 0; i < eyesAll.size(); i++) {
        for (int j = 0; j < eyesAll.size() / 2; j++) {
            if (i == j) continue;
            if (abs(eyesAll[i].y - eyesAll[j].y) < yDiff) {
                eyeFirstIndex = i;
                eyeSecondIndex = j;
                yDiff = abs(eyesAll[i].y - eyesAll[j].y);
            }
        }
    }

    // Face too rotated or incorrect detection
    if (yDiff > face.rows * 0.3) return 0;

    // Push to output
    eyes.push_back(eyesAll[eyeFirstIndex]);
    eyes.push_back(eyesAll[eyeSecondIndex]);

    eyesAll.clear();

    return 1;
}

void Detector::detectForehead(Mat face) {
    this->working = true;
    if(!detectForeheadFromFaceViaEyesDetection(face, this->forehead)) {
        this->forehead = defaultForehead(face);
    }
    this->working = false;
}

bool Detector::isDetected() {
    return (!!this->faces.size() || !!this->forehead.x);
}