#pragma once

#include <vector>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

class Track;
typedef cv::Point TrackingData;
typedef std::vector<cv::Point> Contour;
typedef std::vector<std::unique_ptr<Track>> Tracks;

cv::Point calcCentroid(Contour& contour);

class Track {
public:
    static const int costNonassignment = 100;
    // Will modify the contour vector - whatever remains was not assigned to a track
    static void assignTracks(Tracks& tracks, std::vector<Contour>& contours);
    static size_t getNextIndex();

    Track(Contour& contour, int id = getNextIndex());
    ~Track();

    int getId();
    int getAge();
    int getVisibleCount();
    int getInvisibleAge();
    bool isVisible();
    const cv::Rect& getBBox();
    const Contour& getContour();
    const TrackingData& getPrediction();
    // This track was not assigned a contour, so increase age and mark as invisible
    void update();
    // This track was assigned a contour, so increase age and update filter
    void update(Contour& new_contour);

private:
    static size_t instances;

    int id;
    int age;
    int visibleCount;
    int invisibleAge;
    bool visible;
    cv::Rect bbox;
    Contour contour;
    TrackingData prediction;
    cv::KalmanFilter kalman;
};
