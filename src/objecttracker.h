#pragma once

#include <opencv2/core/mat.hpp>
#include "track.h"

class ObjectTracker {
public:
    virtual ~ObjectTracker();

    virtual void processFrame(cv::UMat& frame) = 0;
    const Tracks& getTracks();
    cv::UMat& getMaskImage();

protected:
    Tracks tracks;
    cv::UMat maskImage;

    void processContours(Tracks& tracks, std::vector<Contour>& contours);
};
