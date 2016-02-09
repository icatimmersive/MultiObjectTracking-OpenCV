# Instructions for Building OpenCV
These are instructions for building the latest OpenCV and required extra modules.
The extra modules are part of the opencv_contrib repository.

## Automatic building
Use the `build.sh` shell script to download and build OpenCV automatically. You will need
C++ build tools, CMake, make, git, and ideally Qt development files installed. By default
the script will ask questions about the build process. Use `--non-interactive` to build
without supervision, check `--help` for more information.

## Manual building
### Download OpenCV and extras
Clone the git repositories:
````
$ git clone --depth=1 https://github.com/Itseez/opencv.git
$ git clone --depth=1 https://github.com/Itseez/opencv_contrib.git
````

### Copy extra modules
To avoid building the entire contrib repo, copy only the required modules to the opencv directory.
````
$ cp -r opencv_contrib/modules/bgsegm opencv/modules
````

### Prepare building
Use CMake to get the build environment ready:
````
$ cd opencv
$ mkdir build
$ cmake -DCMAKE_BUILD_TYPE=Release -DWITH_QT=ON -DWITH_OPENGL=ON ..
````
If you want to build and install the samples also, use the following additional options:
````
-DBUILD_EXAMPLES=ON -DINSTALL_C_EXAMPLES=ON -DINSTALL_PYTHON_EXAMPLES=ON
````
Check the CMake output to make sure the options you want are enabled, and to see if any
required dependencies were not found.

### Build and install OpenCV
````
$ make -j4
$ sudo make install
````