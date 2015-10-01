#include "track.h"

Track::Track(int id, cv::Rect& bbox, Contour& contour) : id(id), bbox(bbox), contour(contour) {
}

int Track::getId() {
    return id;
}

const cv::Rect& Track::getBBox() {
    return bbox;
}

void Track::setBBox(cv::Rect& new_bbox) {
    bbox = new_bbox;
}

const Contour& Track::getContour() {
    return contour;
}

void Track::setContour(Contour& new_contour) {
    contour = new_contour;
}
