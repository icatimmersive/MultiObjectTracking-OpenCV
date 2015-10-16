#include "display.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>

const cv::Scalar maskColor(0, 0, 255); // red
const cv::Scalar bboxColor(0, 255, 0, 127); // green
const cv::Scalar contourColor(0, 255, 255, 127); // yellow
const int contourThickness = 2;
const cv::Scalar predictionColor(255, 0, 0); // blue
const int predictionSize = 3;
const cv::Scalar labelColor(0, 0, 0); // black
const cv::HersheyFonts labelFont = cv::HersheyFonts::FONT_HERSHEY_SIMPLEX;
const double labelScale = 0.4;
const int labelFontThickness = 1;
const int labelPad = 3;
const cv::Scalar fpsColor(255, 255, 255); // white

// FPS

Display::FPS::FPS(int size) : prevTick(0), size(size), index(0), sum(0), total(0) {
    ticks = new std::int64_t[size];
    std::fill(ticks, ticks + size, 0);
}

Display::FPS::~FPS() {
    delete[] ticks;
}

void Display::FPS::update(std::int64_t newTick) {
    if(total < size) {
        total++;
    }
    if(total == 1) {
        prevTick = newTick;
        return;
    }
    std::int64_t tickDiff = newTick - prevTick;
    prevTick = newTick;
    sum -= ticks[index];
    sum += tickDiff;
    ticks[index] = tickDiff;
    index = (index == size - 1 ? 0 : index + 1);
}

double Display::FPS::getFPS() {
    return (cv::getTickFrequency() / (sum * 1.0 / total));
}

// Display

void drawTracks(cv::UMat& image, const Tracks& tracks, cv::Point& mousePos, double fps) {
    for(const std::unique_ptr<Track>& track : tracks) {
        // Draw contour outline
        const Contour& contour = track->getContour();
        cv::drawContours(image, std::vector<Contour>({contour}), 0, contourColor, contourThickness);
        // Draw bounding box
        const cv::Rect& bbox = track->getBBox();
        if(bbox.contains(mousePos)) {
            cv::UMat roi = image(bbox);
            cv::UMat bboxFill(roi.rows, roi.cols, roi.type(), {255, 255, 255});
            cv::addWeighted(bboxFill, 0.5, roi, 0.5, 0.0, roi);
        }
        cv::rectangle(image, bbox.tl(), bbox.br(), bboxColor);
        // Draw current prediction
        cv::circle(image, track->getPrediction(), predictionSize, predictionColor, -1);
        // Annotate bounding box with track information
        std::ostringstream labelTextStream;
        labelTextStream << "Blob " << track->getId() << " (Area: " << cv::contourArea(contour) << ")";
        std::string labelText = labelTextStream.str();
        // - Measure text size
        cv::Size labelSize = cv::getTextSize(labelText, labelFont, labelScale, labelFontThickness, nullptr);
        labelSize.width += 2 * labelPad;
        labelSize.height += 2 * labelPad + 1;
        // - Draw label background
        cv::Rect labelRect({bbox.x, bbox.y - labelSize.height}, labelSize);
        cv::rectangle(image, labelRect.tl(), labelRect.br(), bboxColor, -1);
        // - Draw label text
        cv::Point labelPos(bbox.x + labelPad, bbox.y - labelPad - 1);
        cv::putText(image, labelText, labelPos, labelFont, labelScale, labelColor, labelFontThickness);
    }
    // Draw FPS text
    std::ostringstream fpsTextStream;
    fpsTextStream << "FPS: " << std::fixed << std::setprecision(2) << fps;
    cv::putText(image, fpsTextStream.str(), {0, image.rows - 8}, labelFont, labelScale, fpsColor);
}

void mouseCallback(int event, int x, int y, int flags, void* data) {
    if(event != cv::MouseEventTypes::EVENT_MOUSEMOVE) {
        return;
    }
    cv::Point& point = *(static_cast<cv::Point*>(data));
    point.x = x;
    point.y = y;
}

Display::Display(int cameraId) : fps(100) {
    // Initialize windows
    imageWinTitle = "Camera " + std::to_string(cameraId);
    blobWinTitle = imageWinTitle + " Blobs";
    cv::namedWindow(imageWinTitle);
    cv::namedWindow(blobWinTitle);
    cv::setMouseCallback(imageWinTitle, mouseCallback, (void*) &mousePos);
    cv::setMouseCallback(blobWinTitle, mouseCallback, (void*) &mousePos);
}

Display::~Display() {
    cv::destroyWindow(imageWinTitle);
    cv::destroyWindow(blobWinTitle);
}

void Display::showFrame(cv::UMat& frame, cv::UMat& maskImage, const Tracks& tracks) {
    // Calculate FPS
    fps.update(cv::getTickCount());
    double fpsValue = fps.getFPS();
    // Move windows to be side by side
    cv::moveWindow(imageWinTitle, 0, 0);
    cv::moveWindow(blobWinTitle, frame.cols, 0);
    // Convert 1-channel mask to BGR image
    cv::cvtColor(maskImage, rgbMaskImage, cv::COLOR_GRAY2BGR);
    // Display just the blob and track information
    cv::UMat blobImage = rgbMaskImage.clone();
    drawTracks(blobImage, tracks, mousePos, fpsValue);
    cv::imshow(blobWinTitle, blobImage);
    // Color the mask so it shows up better when overlaid
    rgbMaskImage.setTo(maskColor, maskImage);
    // Add the colored mask image as a semi-transparent overlay to the camera image
    cv::addWeighted(frame, 1.0, rgbMaskImage, 0.65, 0.0, buffer);
    drawTracks(buffer, tracks, mousePos, fpsValue);
    cv::imshow(imageWinTitle, buffer);
}
