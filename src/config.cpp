#include "config.h"
#include <iostream>

Config* Config::instance = nullptr; 

Config::Config(std::string filename) : file(filename, cv::FileStorage::READ) {
    for(cv::FileNode node : file["cameras"]) {
        int id;
        node["id"] >> id;
        CameraInfo info = {id,
                           node["name"],
                           node["desc"],
                           node["class"],
                           node["ip"],
                           {}, {}};
        node["crop"] >> info.crop;
        // Check if spawn regions are defined for this camera
        cv::FileNode sNode = node["spawns"];
        if(sNode.isSeq()) {
            info.spawns.resize(sNode.size());
            for(int i = 0; i < sNode.size(); i++) {
                sNode[i] >> info.spawns[i];
            }
        }
        cameras[id] = info;
    }
}

Config::~Config() {
}

Config& Config::get() {
    if(!instance) {
        instance = new Config("config.yml");
    }
    return *instance;
}

std::string Config::getServerURL() {
    return file["server"]["url"];
}

int Config::getServerPort() {
    int port;
    file["server"]["port"] >> port;
    return port;
}

bool Config::isCameraDefined(int id) {
    return cameras.count(id) > 0;
}

CameraInfo Config::getCameraInfo(int id) {
    return cameras[id];
}

CameraClass Config::getCameraClassInfo(std::string name) {
    cv::FileNode node = file["classes"][name];
    CameraClass classInfo = {name,
                             node["username"],
                             node["password"],
                             node["path"]};
    return classInfo;
}

CameraClass Config::getCameraClassInfo(int camId) {
    std::string className = getCameraInfo(camId).className;
    return getCameraClassInfo(className);
}
