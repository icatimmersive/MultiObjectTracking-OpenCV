#include "display.h"
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

const cv::Scalar maskColor(0, 0, 255); // red
const cv::Scalar bboxColor(0, 255, 0, 127); // green
const cv::Scalar contourColor(0, 255, 255, 127); // yellow
const int contourThickness = 2;

void drawTracks(cv::UMat& image, const Tracks& tracks) {
    for(const std::unique_ptr<Track>& track : tracks) {
        cv::drawContours(image, std::vector<Contour>({track->getContour()}), 0, contourColor, contourThickness);
        const cv::Rect& bbox = track->getBBox();
        cv::rectangle(image, bbox.tl(), bbox.br(), bboxColor);
    }
}

Display::Display(int cameraId) {
    // Initialize windows
    imageWinTitle = "Camera " + std::to_string(cameraId);
    blobWinTitle = imageWinTitle + " Blobs";
    cv::namedWindow(imageWinTitle);
    cv::namedWindow(blobWinTitle);
}

Display::~Display() {
    cv::destroyWindow(imageWinTitle);
    cv::destroyWindow(blobWinTitle);
}

void Display::showFrame(cv::UMat& frame, cv::UMat& maskImage, const Tracks& tracks) {
    // Move windows to be side by side
    cv::moveWindow(imageWinTitle, 0, 0);
    cv::moveWindow(blobWinTitle, frame.cols, 0);
    // Resize buffers to correct size
    buffer.create(frame.rows, frame.cols, frame.depth());
    rgbMaskImage.create(maskImage.rows, maskImage.cols, frame.depth());
    // Convert 1-channel mask to BGR image
    cv::cvtColor(maskImage, rgbMaskImage, cv::COLOR_GRAY2BGR);
    // Display just the blob and track information
    cv::UMat blobImage = rgbMaskImage.clone();
    drawTracks(blobImage, tracks);
    cv::imshow(blobWinTitle, blobImage);
    // Color the mask so it shows up better when overlaid
    rgbMaskImage.setTo(maskColor, maskImage);
    // Add the colored mask image as a semi-transparent overlay to the camera image
    cv::addWeighted(frame, 1.0, rgbMaskImage, 0.65, 0.0, buffer);
    drawTracks(buffer, tracks);
    cv::imshow(imageWinTitle, buffer);
}
