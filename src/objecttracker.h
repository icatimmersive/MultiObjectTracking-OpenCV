#pragma once

#include <vector>
#include <unordered_set>
#include <opencv2/core/mat.hpp>
#include "track.h"
#include "camera.h"

class ObjectTracker {
public:
    virtual ~ObjectTracker();

    virtual void processFrame(cv::UMat& frame, const Spawns& spawns) = 0;
    const Tracks& getTracks();
    // Will clear the list of deleted tracks, so subsequent calls return an empty list.
    const std::unordered_set<int> getDeletedTracks();
    cv::UMat& getMaskImage();

protected:
    Tracks tracks;
    Tracks tracks_sec;
    std::unordered_set<int> deletedTracks;
    std::unordered_set<int> deletedTracks_sec;
    cv::UMat maskImage;

    void processContours(Tracks& tracks, std::vector<Contour>& contours, const Spawns& spawns,Tracks& tracks_sec);
};
