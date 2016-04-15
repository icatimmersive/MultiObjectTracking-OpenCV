#include "camera.h"
#include "config.h"

Camera::Camera(int id, std::string url) : id(id), video(url) {
    Config& config = Config::get();
    camDefined = config.isCameraDefined(id);
    if(camDefined) {
        CameraInfo camInfo = config.getCameraInfo(id);
        crop = camInfo.crop;
        spawns = camInfo.spawns;
    }
}

int Camera::getId() {
    return id;
}

bool Camera::getFrame(cv::UMat& frame) {
    bool status = video.read(frame);
    if(!status) {
        return false;
    }
    if(camDefined) {
        frame = frame(crop);
    }
    return true;
}

Spawns& Camera::getSpawns() {
    return spawns;
}
