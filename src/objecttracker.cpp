#include "objecttracker.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <iterator>
#include <opencv2/flann/flann.hpp>
#include "track.h"

const int kdTreeLeafMax = 3;
const int nnSearchRadius = 50;
const int nnMaxResults = 50;

const int invisibleMax = 5;
const double visibleThreshold = 0.6;

bool isTrackLost(std::unique_ptr<Track>& track) {
    double visiblePercent = track->getVisibleCount() * 1.0 / track->getAge();
    return (visiblePercent < visibleThreshold || track->getInvisibleAge() > invisibleMax);
}

// Using FLANN to group contours together by centroid
std::vector<Contour> combineContours(std::vector<Contour>& contours) {
    std::vector<Contour> combined;

    // Calculate centroids
    std::vector<cv::Point> contourPoints(contours.size());
    std::transform(contours.begin(), contours.end(), contourPoints.begin(), calcCentroid);

    // Perform nearest-neighbor search
    std::cout << cv::Mat(contourPoints) << std::endl;
    cv::flann::KDTreeIndexParams indexParams(kdTreeLeafMax);
    cv::flann::Index kdTree(contourPoints, indexParams);
    std::vector<int> indices;
    std::vector<float> dists;
    kdTree.radiusSearch(contourPoints, indices, dists, nnSearchRadius, nnMaxResults);
    for(int i : indices) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

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
    
    // Assign contours to existing tracks
    Track::assignTracks(tracks, contours);
    // Create new tracks for unassigned contours
    for(Contour& contour : contours) {
        tracks.emplace_back(new Track(contour));
    }
}
