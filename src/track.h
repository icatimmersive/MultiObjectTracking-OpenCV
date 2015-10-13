#pragma once

#include <vector>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

class Track;
typedef cv::Point TrackingData;
typedef std::vector<cv::Point> Contour;
typedef std::vector<std::unique_ptr<Track>> Tracks;

class Track {
public:
    static const int costNonassignment = 20;
    // Will modify the contour vector - whatever remains was not assigned to a track
    static void assignTracks(Tracks& tracks, std::vector<Contour>& contours);
    Track(int id, Contour& contour);

    int getId();
    const cv::Rect& getBBox();
    const Contour& getContour();
    const TrackingData& getPrediction();
    void update(Contour& new_contour);

private:
    int id;
    cv::Rect bbox;
    Contour contour;
    TrackingData prediction;
    cv::KalmanFilter kalman;
};
