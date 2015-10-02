# Multi-Object Tracking with OpenCV
This is an initial port of the existing difference-tracking MATLAB code. The program is split into different
classes for modularity. It is possible to have multiple implementation of object tracking, with a
difference-subtracton tracker (almost exactly the one implemented in MATLAB) being the only current implementation.

## Building
You will need OpenCV 3 (not version 2.4.11 or anything previous). It can be downloaded from the
[OpenCV download page](http://opencv.org/downloads.html). Linux/Mac users will most likely need to build it from
source, unless you can find prebuilt binaries for your system. Once installed, use the CMake gui to build the
program, or build from the terminal:
````
$ mkdir build        # Create a build directory
$ cd build
$ cmake ..           # Use CMake to generate the Makefile
$ make -j4           # Compiles four files in parallel, best for quad-core computers
````
The final binary is `mw-tracking` and will be placed in the `build` directory. You can replace the 4 in `-j4` with
two times the number of CPU cores you have for possibly faster compiling.

## Running
The syntax for running the program is `./mw-tracking <id> <url>`, where `<id>` is the camera's ID number and `<url>`
is the URL to the camera or to a video file.
