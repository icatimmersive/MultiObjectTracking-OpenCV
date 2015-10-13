#include "difference.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <opencv2/video.hpp>

const double contourMinArea = 400.0;

void smoothMask(cv::UMat& maskImage) {
    // Median filter for smoothing
    cv::medianBlur(maskImage, maskImage, 3); // MATLAB code uses medfilt2, which uses 3x3 aperture
    // Morphological operations to remove noise and fill in holes
    cv::Mat kernel = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(10, 8));
    cv::morphologyEx(maskImage, maskImage, cv::MorphTypes::MORPH_CLOSE, kernel);
}

bool contourFilter(Contour& contour) {
    double contourArea = cv::contourArea(contour);
    return contourArea >= contourMinArea;
}

DifferenceTracker::DifferenceTracker() {
    // Initialize difference engine and blob detector
    // Parameters taken from original MATLAB code
//     diffEngine = cv::bgsegm::createBackgroundSubtractorMOG(100, 2, 0.01); // requires contrib module "bgsegm"
//     diffEngine = cv::createBackgroundSubtractorKNN(1000, 400.0, false);
    diffEngine = cv::createBackgroundSubtractorMOG2(100, 400.0, false);
}

DifferenceTracker::~DifferenceTracker() {
}

void DifferenceTracker::processFrame(cv::UMat& frame) {
    // third parameter = rate of background update (0.0 is no update)
    diffEngine->apply(frame, maskImage, 0.0);
    // Apply operations to improve mask image
    smoothMask(maskImage);
    // Make a copy of the mask because findContours will modify it
    cv::UMat maskImageCopy = maskImage.clone();
    std::vector<Contour> contours;
    // RETR_EXTENRAL = find outermost contours, CHAIN_APPROX_SIMPLE = approximate lines to reduce number of points
    cv::findContours(maskImageCopy, contours, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
    // For now (testing), destroy all track objects each time
    tracks.clear();
    int i = 0;
    for(Contour contour : contours) {
        if(!contourFilter(contour)) {
            continue;
        }
        cv::Rect bbox = cv::boundingRect(contour);
        std::unique_ptr<Track> track(new Track(i, bbox, contour));
        tracks.push_back(std::move(track));
        i++;
    }
}
