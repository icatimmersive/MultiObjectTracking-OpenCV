#pragma once

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

typedef std::vector<cv::Rect> Spawns;

class Camera {
public:
    Camera(int id, std::string url);

    int getId();
    bool getFrame(cv::UMat& frame);
    Spawns& getSpawns();

private:
    int id;
    cv::VideoCapture video;
    cv::Rect crop;
    Spawns spawns;
    bool camDefined;
};
