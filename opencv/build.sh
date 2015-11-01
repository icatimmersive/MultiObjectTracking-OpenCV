#! /bin/bash

function echo1() {
	echo -e "\n\033[1;32m$1\033[0m"
}

function echo1n() {
	echo -en "\n\033[1;32m$1\033[0m"
}

function echo2() {
	echo -e "\033[0;32m$1\033[0m"
}

function echoE() {
	echo -e "\033[1;31m$1\033[0m"
}

CONTRIBS="bgsegm"
CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Release -DWITH_QT=ON -DWITH_OPENGL=ON"
CMAKE_SAMPLES="-DBUILD_EXAMPLES=ON -DINSTALL_C_EXAMPLES=ON -DINSTALL_PYTHON_EXAMPLES=ON"
PACKAGES="git cmake build-essential qt5-default python-dev python3-dev python-numpy python3-numpy default-jdk default-jre ant libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavresample-dev libv4l-dev"

if which apt-get > /dev/null; then
	echo1n "Install required packages? [Y/n] "
	read INSTALL
	if [ "x$INSTALL" != "xn" -a "x$INSTALL" != "xN" ]; then
		echo "Installing packages..."
		sudo apt-get -y install $PACKAGES
	fi
fi

echo1n "Build samples? [Y/n] "
read SAMPLES
if [ "x$SAMPLES" != "xn" -a "x$SAMPLES" != "xN" ]; then
	echo "Will build samples"
	CMAKE_OPTIONS="$CMAKE_OPTIONS $CMAKE_SAMPLES"
else
	echo "Not building samples"
fi

echo1 "Downloading OpenCV..."
if [ -d opencv ]; then
	cd opencv
	git pull
else
	git clone --depth=1 https://github.com/Itseez/opencv.git
fi

echo1 "Downloading OpenCV extra modules..."
if [ -d opencv_contrib ]; then
	cd opencv_contrib
	git pul
else
	git clone --depth=1 https://github.com/Itseez/opencv_contrib.git
fi

echo1 "Copying only required extra modules..."
for mod in $CONTRIBS; do
	echo2 " - $mod"
	cp -r opencv_contrib/modules/$mod opencv/modules
done

echo1 "Preparing OpenCV build..."
cd opencv
mkdir build
cd build
cmake $CMAKE_OPTIONS ..

echo1 "Press [ENTER] to begin building"
read
make -j4
if [ "x$?" != "x0" ]; then
	echoE "Error building OpenCV"
	exit
fi

echo1 "Installing OpenCV..."
sudo make install
