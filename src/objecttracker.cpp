#include "objecttracker.h"
#include <memory>
#include <algorithm>

const int invisibleMax = 20;
const double visibleThreshold = 0.6;

bool isTrackLost(std::unique_ptr<Track>& track) {
    double visiblePercent = track->getVisibleCount() * 1.0 / track->getAge();
    return (visiblePercent < visibleThreshold || track->getInvisibleAge() > invisibleMax);
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
    for(auto it = tracks.begin(); it != tracks.end();) {
        std::unique_ptr<Track>& track = *it;
        if(isTrackLost(track)) {
            deletedTracks.insert(track->getId());
            it = tracks.erase(it);
        } else {
            it++;
        }
    }
    // Assign contours to existing tracks
    Track::assignTracks(tracks, contours);
    // Create new tracks for unassigned contours
    for(Contour& contour : contours) {
        tracks.emplace_back(new Track(contour));
    }
}
