#pragma once

#include <cstdint>
#include <string>
#include <opencv2/core/mat.hpp>
#include "track.h"

class Display {
public:
    Display(int cameraId);
    ~Display();

    void showFrame(cv::UMat& frame, cv::UMat& maskImage, const Tracks& tracks);

private:
    class FPS {
    public:
        FPS(int size);
        ~FPS();

        void update(std::int64_t newTick);
        double getFPS();

    private:
        std::int64_t prevTick;
        int size;
        int index;
        int total;
        std::uint64_t sum;
        std::int64_t *ticks;
    };

    std::string imageWinTitle;
    std::string blobWinTitle;
    cv::UMat buffer;
    cv::UMat rgbMaskImage;
    cv::Point mousePos;
    FPS fps;
};
