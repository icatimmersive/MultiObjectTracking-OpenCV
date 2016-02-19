#include "camera.h"
#include "config.h"

Camera::Camera(int id, std::string url) : id(id), video(url) {
    Config& config = Config::get();
    cropping = config.isCameraDefined(id);
    if(cropping) {
        CameraInfo camInfo = config.getCameraInfo(id);
        crop = camInfo.crop;
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
    if(cropping) {
        frame = frame(crop);
    }
    return true;
}
