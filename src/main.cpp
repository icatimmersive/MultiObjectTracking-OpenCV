#include <string>
#include <iostream>
#include <unistd.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "camera.h"
#include "display.h"
#include "objecttracker.h"
#include "trackers/difference.h"

void printUsage(std::string progName) {
    std::cout << "Usage: " << progName << " <id> <url or file>" << std::endl;
}

int main(int argc, char *argv[]) {
    int id;
    std::string url;

    // Read arguments and set parameters
    if(argc == 1) {
        // Defaults
        id = 0;
//         url = "../walk.mov";
        url = "http://admin:admin@192.168.1.14/video.cgi?.mjpg";
    } else if(argc != 3 || (argc == 2 && std::string(argv[1]) == "--help")) {
        printUsage(argv[0]);
        return 0;
    }

    // Initialize system objects
    Camera camera(id, url);
    Display display(id);
    ObjectTracker* tracker = new DifferenceTracker();

    // Start video processing
    cv::UMat frame;
    while(camera.getVideo().read(frame)) {
        tracker->processFrame(frame);
        display.showFrame(frame, tracker->getMaskImage(), tracker->getTracks());
        // Only the least-signficant byte is used, sometimes the rest is garbage so 0xFF is needed
        int key = cv::waitKey(10) & 0xFF;
        if(key == 27) { // Escape pressed
            break;
        }
    }
}
