#include "track.h"
#include <cmath>
#include <opencv2/imgproc.hpp>
#include <iostream>
size_t Track::instances = 0;
TrackingData prevdata ;
cv::Point calcCentroid(Contour& contour) {
    cv::Moments moments = cv::moments(contour);
    double xc = moments.m10 / moments.m00;
    double yc = moments.m01 / moments.m00;
    return {(int)std::round(xc), (int)std::round(yc)};
}

double getAssignCost(std::unique_ptr<Track>& track, Contour& contour) {
    TrackingData data = calcCentroid(contour);
    if((data.x == prevdata.x) && (data.y ==prevdata.y))
        return 0;
    const TrackingData& prediction = track->getPrediction();
    return sqrt(pow(data.x - prediction.x, 2) + pow(data.y - prediction.y, 2));
    prevdata = data;
}

void Track::assignTracks(Tracks& tracks, std::vector<Contour>& contours) {
    // TODO Implement Munkres/Hungarian algorithm to solve
    // the cost assignment problem in O(n^3) rather t
    int trackno=0;
    for(std::unique_ptr<Track>& track : tracks) {
        double minCost = costNonassignment;
        int minIndex = -1;
        for(int i = 0; i < contours.size(); i++) {
            double cost = getAssignCost(track, contours[i]);
            std::cout<<"Cost"<<"\t"<<cost<<std::endl;    
            if(cost <= minCost && cost !=0) {
                minCost = cost;
                minIndex = i;
            }
            if(costChangeAssignment<cost && track->getAge() > ageChangeAssignment)
            {
                minIndex = i;
            }
        }
        std::cout<<"age of track"<<"\t"<<track->getAge()<<std::endl;
        if(contours.size()==0 && track->getAge() > ageTrackDelete)
        {
            tracks.erase(tracks.begin()+trackno);
            break;
        }

        if(minIndex == -1) {
            // No contour found, so track becomes invisible
            track->update();
        } else {
            track->update(contours[minIndex]);
            contours.erase(contours.begin() + minIndex);
        }
        std::cout<<"Counter Size"<<"\t"<<contours.size()<<std::endl;
        trackno++;
    }
}

size_t Track::getNextIndex() {
    return instances + 1;
}

Track::Track(Contour& contour, int id) : id(id), age(0), visibleCount(0), invisibleAge(0), visible(true) {
    instances++;
    update(contour);
}

Track::~Track() {
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
