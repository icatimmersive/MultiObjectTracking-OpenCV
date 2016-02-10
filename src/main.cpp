#include <cstdlib>
#include <string>
#include <memory>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "camera.h"
#include "display.h"
#include "track.h"
#include "objecttracker.h"
#include "trackers/difference.h"
#include "Networking/blob.h"
#include "Networking/blobSender.h"
#include <opencv2/calib3d.hpp>

const std::string serverURL = "dev.mirrorworlds.icat.vt.edu";
const int serverPort        = 9999;

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
    std::cout << "Usage: " << progName << " <id> <ip or url or file>" << std::endl;
}

std::string parseURL(int camId, std::string arg) {
    in_addr ipaddr;
    if(inet_pton(AF_INET, arg.c_str(), &ipaddr) == 1) {
        std::cout << "Using camera at address " << arg << std::endl;
        if(camId < 6) {
            return std::string("http://admin:admin@") + arg + std::string("/video.cgi?.mjpg");
        } else {
            return std::string("http://root:admin@") + arg + std::string("/video.mjpg");
        }
    } else {
        std::cout << "Using file/URL " << arg << std::endl;
        return arg;
    }
}

void cropImage(int camId, cv::UMat& frame) {
    cv::Rect rectCrop;
    switch(camId) {
    case 1:
        rectCrop = {100, 100, 900, 700};
        break;
    case 2:
        rectCrop = {130, 150, 800, 540};
        break;
    case 3:
        rectCrop = {130, 260, 800, 550};
        break;
    case 4:
        rectCrop = {130, 180, 800, 570};
        break;
    case 5:
        rectCrop = {130, 100, 800, 570};
        break;
    case 6:
        rectCrop = {180, 90, 800, 800};
        break; 
    case 7:
        rectCrop = {180, 240, 750, 650};
        break;
    case 8:
        rectCrop = {180, 250, 750, 650};
        break;
    case 9:
        rectCrop = {180, 250, 750, 800};
        break;
    case 10:
        rectCrop = {180, 250, 750, 726};
        break;
    default:
        return;
    }
    cv::UMat imCrop = frame(rectCrop);
    frame = imCrop;
}

int main(int argc, char *argv[]) {
    int id;
    std::string url;
    // Read arguments and set parameters
    if(argc == 1) {
        // Defaults
        std::cout << "Using default video input" << std::endl;
        id = 0;
        url = "../walk-cut.mov";
    } else if(argc != 3 || (argc == 2 && std::string(argv[1]) == "--help")) {
        printUsage(argv[0]);
        return 0;
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
        while(camera.getVideo().read(frame)) {
            cropImage(id, frame);
            //cv::cvtColor(frame,frame,CV_BGR2HSV);     //hsv SPACE TO ELIMINATE THE SHADOW EFFECT
//by nuo 20151025 unwarping
/*
cv::Mat camera_matrix, distortion;
cv::FileStorage fs("camparam.txt", cv::FileStorage::READ);
cv::FileNode fn = fs["IntParam"];
fn["camera_matrix"] >> camera_matrix;
fn["distortion"] >> distortion;
cv::fisheye::undistortImage(frame,frame,camera_matrix, distortion, camera_matrix);
*/
//nuo end
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
