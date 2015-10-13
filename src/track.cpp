#include "track.h"
#include <cmath>
#include <opencv2/imgproc.hpp>

cv::Point calcCentroid(Contour& contour) {
    cv::Moments moments = cv::moments(contour);
    double xc = moments.m10 / moments.m00;
    double yc = moments.m01 / moments.m00;
    return {(int)std::round(xc), (int)std::round(yc)};
}

Track::Track(int id, Contour& contour) : id(id) {
    update(contour);
}

int Track::getId() {
    return id;
}

const cv::Rect& Track::getBBox() {
    return bbox;
}

const Contour& Track::getContour() {
    return contour;
}

const TrackingData& Track::getPrediction() {
    return prediction;
}

void Track::update(Contour& new_contour) {
    contour = new_contour;
    bbox = cv::boundingRect(contour);
    cv::Point center = calcCentroid(contour);
    // TODO use Kalman filter
    prediction = center;
}
