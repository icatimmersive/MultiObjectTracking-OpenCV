#pragma once

#include <vector>
#include <memory>
#include <opencv2/core.hpp>

typedef std::vector<cv::Point> Contour;

class Track {
public:
    Track(int id, cv::Rect& bbox, Contour& contour);

    int getId();
    const cv::Rect& getBBox();
    void setBBox(cv::Rect& new_bbox);
    const Contour& getContour();
    void setContour(Contour& new_contour);

private:
    int id;
    cv::Rect bbox;
    Contour contour;
};

typedef std::vector<std::unique_ptr<Track>> Tracks;
