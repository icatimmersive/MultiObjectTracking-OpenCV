#pragma once

#include <vector>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

typedef cv::Point TrackingData;
typedef std::vector<cv::Point> Contour;

class Track {
public:
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

typedef std::vector<std::unique_ptr<Track>> Tracks;
