#pragma once

#include <vector>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

typedef std::vector<cv::Point> Contour;
typedef cv::Point Prediction;

class Track {
public:
    Track(int id, Contour& contour);

    int getId();
    const cv::Rect& getBBox();
    const Contour& getContour();
    const cv::Point& getPrediction();
    void update(Contour& new_contour);

private:
    int id;
    cv::Rect bbox;
    Contour contour;
    Prediction prediction;
    cv::KalmanFilter kalman;
};

typedef std::vector<std::unique_ptr<Track>> Tracks;
