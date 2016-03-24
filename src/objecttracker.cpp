#include "objecttracker.h"
#include <cmath>
#include <iostream>
#include <memory>
#include <iterator>
#include <algorithm>
#include <functional>
#include <opencv2/flann/flann.hpp>
#include "track.h"

const int kdTreeLeafMax = 2;
const int nnMaxResults = 4;
const double nnSearchRadius = 20; // 20 pixels around contour edges

const int invisibleMax = 5;
const double visibleThreshold = 0.6;

bool isTrackLost(std::unique_ptr<Track>& track) {
    double visiblePercent = track->getVisibleCount() * 1.0 / track->getAge();
    return (visiblePercent < visibleThreshold || track->getInvisibleAge() > invisibleMax);
}

// Using FLANN to group contours together by centroid
std::vector<Contour> combineContours(std::vector<Contour>& contours) {
    std::vector<Contour> combined;

    // Sort contours by area
    std::sort(contours.begin(), contours.end(), [](const Contour& a, const Contour& b) {return cv::contourArea(a) < cv::contourArea(b);});

    // Cache contour data (enclosing circle center and radius)
    std::vector<Contour*> contoursCopy(contours.size());
    std::vector<cv::Point2f> contourCenters;
    std::vector<double> contourRadii;
    std::transform(contours.begin(), contours.end(), contoursCopy.begin(), [](Contour& c) {return &c;});
    for(Contour& c : contours) {
        cv::Point2f center;
        float radius;
        cv::minEnclosingCircle(c, center, radius);
        contourCenters.push_back(center);
        contourRadii.push_back(radius);
    }

    // Perform nearest-neighbor search on biggest blobs until none left
    cv::flann::KDTreeIndexParams indexParams(kdTreeLeafMax);
    if(contours.size() > 1) {
        // Get last contour and data
        Contour& query = *contoursCopy.back();
        cv::Point2f queryPoint = contourCenters.back();
        double queryRadius = contourRadii.back();
        contoursCopy.pop_back();
        contourCenters.pop_back();
        contourRadii.pop_back();

        // Build KD-tree
        // Default distance is L2 distance, which is not Eucidean, it is Euclidean squared
        cv::flann::Index kdTree(cv::Mat(contourCenters).reshape(1), indexParams);
        std::vector<int> indices;
        std::vector<float> dists;
        // Find k nearest neighbors
        kdTree.knnSearch(cv::Matx12f(queryPoint.x, queryPoint.y), indices, dists, nnMaxResults);
        Contour combinedContour = query;
        int total = std::min((int)contourCenters.size(), nnMaxResults);
        std::cout << total << ": ";
        for(int i = 0; i < total; i++) {
            std::cout << indices[i];
            double distance = std::sqrt(dists[i]);
            // Estimate threshold distance between contours using radii of enclosing circles
            double nbrRadius = contourRadii[indices[i]];
            double maxDistance = queryRadius + nbrRadius + nnSearchRadius;
            std::cout << "-(dist: " << distance << ", max: " << maxDistance << ")";
            if(distance <= maxDistance) {
                // If under threshold distance, then combine contours
                std::cout << "-combine";
                Contour& nbrContour = *contoursCopy[indices[i]];
                combinedContour.insert(combinedContour.end(), nbrContour.begin(), nbrContour.end());
            }
            std::cout << ", ";
        }
        std::cout << std::endl;

        Contour combinedHull;
        cv::convexHull(combinedContour, combinedHull);
        combined.push_back(combinedHull);
    }

    return combined;
}

ObjectTracker::~ObjectTracker() {
}

const Tracks& ObjectTracker::getTracks() {
    return tracks;
}

const std::unordered_set<int> ObjectTracker::getDeletedTracks() {
    std::unordered_set<int> copy(deletedTracks);
    deletedTracks.clear();
    return copy;
}

cv::UMat& ObjectTracker::getMaskImage() {
    return maskImage;
}

void ObjectTracker::processContours(Tracks& tracks, std::vector<Contour>& contours) {
    // Delete lost tracks:
    // Tracks that are too sporadically visible, or have been invisible for too long
    // TODO delete with heuristics
    for(auto it = tracks.begin(); it != tracks.end();) {
        std::unique_ptr<Track>& track = *it;
        if(isTrackLost(track)) {
            deletedTracks.insert(track->getId());
            it = tracks.erase(it);
        } else {
            it++;
        }
    }

    std::vector<Contour> combinedContours = combineContours(contours);
    contours = combinedContours;
    
    // Assign contours to existing tracks
    Track::assignTracks(tracks, contours);
    // Create new tracks for unassigned contours
    for(Contour& contour : contours) {
        tracks.emplace_back(new Track(contour));
    }
}
