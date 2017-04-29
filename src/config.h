#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <opencv2/core.hpp>
#include "camera.h"

struct CameraClass {
    std::string name;
    std::string username;
    std::string password;
    std::string path;
};

struct CameraInfo {
    int id;
    std::string name;
    std::string description;
    std::string className;
    std::string ip;
    cv::Rect crop;
    Spawns spawns;
};

class Config {
public:
    static Config& get();

    ~Config();

    std::string getServerURL();
    int getServerPort();

    bool isCameraDefined(int id);
    CameraInfo getCameraInfo(int id);

    CameraClass getCameraClassInfo(std::string name);
    CameraClass getCameraClassInfo(int camId);
private:
    static Config* instance;

    cv::FileStorage file;
    std::unordered_map<int, CameraInfo> cameras;

    Config(std::string filename);
};
