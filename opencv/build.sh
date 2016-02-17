#! /bin/bash

function echo1() {
	echo -e "\033[1;32m$1\033[0m"
}

function echo1n() {
	echo -en "\033[1;32m$1\033[0m"
}

function echo2() {
	echo -e "\033[0;32m$1\033[0m"
}

function echoE() {
	echo -e "\033[1;31m$1\033[0m"
}

CONTRIBS="bgsegm"
CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Release -DWITH_QT=ON -DWITH_OPENGL=ON"
CMAKE_CONTRIB_OPTIONS="-DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules"
CMAKE_SAMPLES="-DBUILD_EXAMPLES=ON -DINSTALL_C_EXAMPLES=ON -DINSTALL_PYTHON_EXAMPLES=ON"
PACKAGES="git cmake build-essential qt5-default python-dev python3-dev python-numpy python3-numpy default-jdk default-jre ant libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavresample-dev libv4l-dev"

INSTALL="n"
SAMPLES="n"
CONTRIB="n"

while [ "x$#" != "x0" ]; do
	case "x$1" in
	"x--help")
		echo1 "Build script for OpenCV 3.x"
		echo ""
		echo "Usage: $0 [--non-interactive [--install-packages] [--build-samples]] [--build-all-contrib]"
		echo ""
		echo "In non-interactive mode, by default required packages will not be installed and samples will not be built, unless otherwise specified."
		echo "By default, only required extra opencv modules will be built. To build all extra modules, use --build-all-contrib."
		exit
		;;
	"x--non-interactive")
		echo1 "Non-interactive mode"
		NONINT="y"
		;;
	"x--install-packages")
		INSTALL="y"
		;;
	"x--build-samples")
		SAMPLES="y"
		;;
	"x--build-all-contrib")
		CONTRIB="y"
		;;
	esac
	shift
done

echo1n "\nBuild samples? [Y/n] "
if [ "x$NONINT" == "xy" ]; then
	echo $SAMPLES
else
	read SAMPLES
fi
if [ "x$SAMPLES" != "xn" -a "x$SAMPLES" != "xN" ]; then
	echo "Will build samples"
	CMAKE_OPTIONS="$CMAKE_OPTIONS $CMAKE_SAMPLES"
else
	echo "Not building samples"
fi

if which apt-get > /dev/null 2>&1; then
	echo1n "\nInstall required packages? [Y/n] "
	if [ "x$NONINT" == "xy" ]; then
		echo $INSTALL
	else
		read INSTALL
	fi
	if [ "x$INSTALL" != "xn" -a "x$INSTALL" != "xN" ]; then
		echo "Installing packages..."
		# Don't use sudo in non-interactive mode
		if [ "x$NONINT" == "xy" ]; then
			SUDO=""
		else
			SUDO="sudo"
		fi
		$SUDO apt-get -y install $PACKAGES
		if [ "x$NONINT" == "xy" -a "x$?" != "x0" ]; then
			echoE "apt-get failed; maybe requires root"
			exit 1
		fi
	fi
else
	echoE "\nNo apt-get found; required packages will not be installed"
fi

echo1 "\nDownloading OpenCV..."
if [ -d opencv ]; then
	cd opencv
	git pull
	cd ..
else
	git clone --depth=1 https://github.com/Itseez/opencv.git
fi

echo1 "\nDownloading OpenCV extra modules..."
if [ -d opencv_contrib ]; then
	cd opencv_contrib
	git pull
	cd ..
else
	git clone --depth=1 https://github.com/Itseez/opencv_contrib.git
fi

if [ "x$CONTRIB" == "xy" ]; then
	echo1 "\nBuilding all extra modules"
	CMAKE_OPTIONS="$CMAKE_OPTIONS $CMAKE_CONTRIB_OPTIONS"
else
	echo1 "\nCopying only required extra modules..."
	for mod in $CONTRIBS; do
		echo2 " - $mod"
		cp -r opencv_contrib/modules/$mod opencv/modules
	done
fi

echo1 "\nPreparing OpenCV build..."
cd opencv
mkdir build
cd build
cmake $CMAKE_OPTIONS ..

echo1 "\nPress [ENTER] to begin building"
if [ "x$NONINT" != "xy" ]; then
	read
fi
NUMCPUS=`grep -c '^processor' /proc/cpuinfo`
make -j$NUMCPUS
if [ "x$?" != "x0" ]; then
	echoE "Error building OpenCV"
	exit
fi

echo1 "Installing OpenCV..."
sudo make install
