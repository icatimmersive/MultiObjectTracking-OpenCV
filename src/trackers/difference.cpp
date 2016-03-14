#include "difference.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <opencv2/video.hpp>

const int skipFrames = 150;
const double contourMinArea = 1200.0;
const double contourMaxArea = 20000.0;

void smoothMask(cv::UMat& maskImage) {
    // Median filter for smoothing
    cv::medianBlur(maskImage, maskImage, 3); // MATLAB code uses medfilt2, which uses 3x3 aperture
    // Morphological operations to remove noise and fill in holes
    cv::Mat kernel = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(14, 14));
    cv::morphologyEx(maskImage, maskImage, cv::MorphTypes::MORPH_OPEN, kernel);
    cv::morphologyEx(maskImage, maskImage, cv::MorphTypes::MORPH_DILATE, kernel);
}

// Returns true if the contour is usable
bool contourFilter(Contour& contour) {
    double contourArea = cv::contourArea(contour);
    return ((contourArea >= contourMinArea) && (contourArea < contourMaxArea));
}

DifferenceTracker::DifferenceTracker() : skipped(0) {
    // Initialize difference engine and blob detector
    // Parameters taken from original MATLAB code
//     diffEngine = cv::bgsegm::createBackgroundSubtractorMOG(100, 2, 0.01); // requires contrib module "bgsegm"
    diffEngine = cv::createBackgroundSubtractorKNN(500, 400.0, false);
//     diffEngine = cv::createBackgroundSubtractorMOG2(100, 400.0, true);
}

DifferenceTracker::~DifferenceTracker() {
}

void DifferenceTracker::processFrame(cv::UMat& frame) {
    // third parameter = rate of background update (0.0 is no update)
    diffEngine->apply(frame, maskImage, 0.0005);
    // Apply operations to improve mask image
    smoothMask(maskImage);
    // Now return if this frame should be skipped
    if(skipped < skipFrames) {
        skipped++;
        return;
    }
    // Make a copy of the mask because findContours will modify it
    cv::UMat maskImageCopy = maskImage.clone();
    std::vector<Contour> contours;
    // RETR_EXTENRAL = find outermost contours, CHAIN_APPROX_SIMPLE = approximate lines to reduce number of points
    cv::findContours(maskImageCopy, contours, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
    contours.erase(std::remove_if(contours.begin(), contours.end(), std::not1(std::ref(contourFilter))), contours.end());
    processContours(tracks, contours);
}
