#include "track.h"
#include <cmath>
#include <opencv2/imgproc.hpp>

size_t Track::instances = 0;

cv::Point calcCentroid(Contour& contour) {
    cv::Moments moments = cv::moments(contour);
    double xc = moments.m10 / moments.m00;
    double yc = moments.m01 / moments.m00;
    return {(int)std::round(xc), (int)std::round(yc)};
}

double getAssignCost(std::unique_ptr<Track>& track, Contour& contour) {
    TrackingData data = calcCentroid(contour);
    const TrackingData& prediction = track->getPrediction();
    return sqrt(pow(data.x - prediction.x, 2) + pow(data.y - prediction.y, 2));
}

void Track::assignTracks(Tracks& tracks, std::vector<Contour>& contours) {
    // TODO Implement Munkres/Hungarian algorithm to solve
    // the cost assignment problem in O(n^3) rather t
    for(std::unique_ptr<Track>& track : tracks) {
        double minCost = costNonassignment;
        int minIndex = -1;
        for(int i = 0; i < contours.size(); i++) {
            double cost = getAssignCost(track, contours[i]);
            if(cost < minCost) {
                minCost = cost;
                minIndex = i;
            }
        }
        if(minIndex == -1) {
            // No contour found, so track becomes invisible
            track->update();
        } else {
            track->update(contours[minIndex]);
            contours.erase(contours.begin() + minIndex);
        }
    }
}

size_t Track::getNextIndex() {
    return instances + 1;
}

Track::Track(Contour& contour, int id) : id(id) {
    instances++;
    update(contour);
}

int Track::getId() {
    return id;
}

int Track::getAge() {
    return age;
}

int Track::getVisibleCount() {
    return visibleCount;
}

int Track::getInvisibleAge() {
    return invisibleAge;
}

bool Track::isVisible() {
    return visible;
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

void Track::update() {
    visible = false;
    age++;
    invisibleAge++;
}

void Track::update(Contour& new_contour) {
    // Update metadata
    visible = true;
    age++;
    visibleCount++;
    invisibleAge = 0;
    // Update filter
    contour = new_contour;
    bbox = cv::boundingRect(contour);
    cv::Point center = calcCentroid(contour);
    // TODO use Kalman filter
    prediction = center;
}
