#include "camera.h"

Camera::Camera(int id, std::string url) : id(id), video(url) {
}

int Camera::getId() {
    return id;
}

cv::VideoCapture& Camera::getVideo() {
    return video;
}
