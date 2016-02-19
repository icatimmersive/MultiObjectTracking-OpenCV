#include <cstdlib>
#include <string>
#include <memory>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "config.h"
#include "camera.h"
#include "display.h"
#include "track.h"
#include "objecttracker.h"
#include "trackers/difference.h"
#include "Networking/blob.h"
#include "Networking/blobSender.h"
#include <opencv2/calib3d.hpp>

void sendTracks(int cameraId, cv::Size imgSize, ObjectTracker* tracker, blobSender& sender) {
    Blob blobData;
    memset(&blobData, 0, sizeof(blobData));
    blobData.cameraID = cameraId;

    // Send deleted blobs first
    for(int id : tracker->getDeletedTracks()) {
        blobData.id = id;
        sender.sendRemoveBlob(&blobData);
    }

    // Then send new/updated blobs
    for(const std::unique_ptr<Track>& track : tracker->getTracks()) {
        blobData.id = track->getId();
        const cv::Rect& bbox = track->getBBox();
        blobData.bounding_x = bbox.x;
        blobData.bounding_y = bbox.y;
        blobData.bounding_width = bbox.width;
        blobData.bounding_height = bbox.height;
        blobData.origin_x = bbox.x + (bbox.width * 0.5);
        blobData.origin_y = bbox.y + (bbox.height * 0.5);
        blobData.area = cv::contourArea(track->getContour());

        blobData.image_width = imgSize.width;
        blobData.image_height = imgSize.height;

        if(track->getAge() == 1) {
            sender.sendNewBlob(&blobData);
        } else {
            sender.sendUpdateBlob(&blobData);
        }
    }
}

void printUsage(std::string progName) {
    std::cout << "Usage: " << progName << " <id> [path]" << std::endl;
    std::cout << std::endl;
    std::cout << "Where [path] is optional and can be an IP address, a URL, or a video file." << std::endl;
    std::cout << "If [path] is an IP address, the correct URL will be guessed based on the camera ID." << std::endl;
    std::cout << "If [path] is omitted, camera parameters will be read from the configuration file." << std::endl;
}

std::string parseURL(int camId, std::string arg) {
    in_addr ipaddr;
    if(inet_pton(AF_INET, arg.c_str(), &ipaddr) == 1) {
        Config& config = Config::get();
        if(config.isCameraDefined(camId)) {
            CameraInfo camInfo = config.getCameraInfo(camId);
            CameraClass classInfo = config.getCameraClassInfo(camInfo.className);
            std::cout << "Using camera " << camId << " (" << camInfo.description << ")" << std::endl;
            std::string url("http://");
            url += classInfo.username + ":" + classInfo.password + "@";
            url += arg + "/" + classInfo.path;
            return url;
        } else {
            std::cout << "Using camera at address " << arg << std::endl;
            return std::string("http://admin:admin@") + arg + std::string("/video.cgi?.mjpg");
        }
    } else {
        std::cout << "Using file/URL " << arg << std::endl;
        return arg;
    }
}

std::string getURL(int camId) {
    CameraInfo camInfo = Config::get().getCameraInfo(camId);
    return parseURL(camId, camInfo.ip);
}

int main(int argc, char *argv[]) {
    std::string serverURL = Config::get().getServerURL();
    int serverPort = Config::get().getServerPort();

    int id;
    std::string url;
    // Read arguments and set parameters
    if(argc == 1) {
        // Defaults
        std::cout << "Using default video input" << std::endl;
        id = 0;
        url = "../walk-cut.mov";
    } else if((argc == 2 && std::string(argv[1]) == "--help") || argc > 3) {
        printUsage(argv[0]);
        return 0;
    } else if(argc == 2) {
        id = std::atoi(argv[1]);
        url = getURL(id);
    } else {
        id = std::atoi(argv[1]);
        url = parseURL(id, argv[2]);
    }

    // Initialize system objects
    Camera camera(id, url);
    Display display(id);
    ObjectTracker* tracker = new DifferenceTracker();
    blobSender sender(serverURL.c_str(), serverPort); // set up networking

    // Start video processing
    try {
        cv::UMat frame;
        while(camera.getFrame(frame)) {
            //cv::cvtColor(frame,frame,CV_BGR2HSV); // Convert to HSV to eliminate shadows
            tracker->processFrame(frame);
            display.showFrame(frame, tracker->getMaskImage(), tracker->getTracks());
            sendTracks(id, {frame.cols, frame.rows}, tracker, sender);
            // Only the least-signficant byte is used, sometimes the rest is garbage so 0xFF is needed
            int key = cv::waitKey(10) & 0xFF;
            if(key == 27) { // Escape pressed
                break;
            }
        }
    } catch(const std::exception& ex) {
        std::cerr << "Error occurred: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
