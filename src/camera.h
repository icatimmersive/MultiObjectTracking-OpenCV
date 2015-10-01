#pragma once

#include <string>
#include <opencv2/videoio.hpp>

class Camera {
public:
    Camera(int id, std::string url);

    int getId();
    cv::VideoCapture& getVideo();

private:
    int id;
    cv::VideoCapture video;
};
