#pragma once

#include <string>
#include <opencv2/core/mat.hpp>
#include "track.h"

class Display {
public:
    Display(int cameraId);
    ~Display();

    void showFrame(cv::UMat& frame, cv::UMat& maskImage, const Tracks& tracks);

private:
    std::string imageWinTitle;
    std::string blobWinTitle;
    cv::UMat buffer;
    cv::UMat rgbMaskImage;
    cv::Point mousePos;
};
