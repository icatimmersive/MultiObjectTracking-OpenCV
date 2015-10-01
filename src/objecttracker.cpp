#include "objecttracker.h"

ObjectTracker::~ObjectTracker() {
}

const Tracks& ObjectTracker::getTracks() {
    return tracks;
}

cv::UMat& ObjectTracker::getMaskImage() {
    return maskImage;
}
