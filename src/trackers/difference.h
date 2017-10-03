#pragma once

#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d.hpp>
#include "../objecttracker.h"

class DifferenceTracker : public ObjectTracker {
public:
    DifferenceTracker();
    ~DifferenceTracker();

    void processFrame(cv::UMat& frame, const Spawns& spawns) override;

private:
    cv::Ptr<cv::BackgroundSubtractor> diffEngine;
    // Skip some of the first few frames while training
    int skipped;
};
