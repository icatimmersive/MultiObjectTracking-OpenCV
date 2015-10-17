#! /bin/sh

function echo1() {
	echo -e "\n\033[1;32m$1\033[0m"
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

echo -n "Build samples? [Y/n] "
read SAMPLES
if [ "x$SAMPLES" != "xn" -a "x$SAMPLES" != "xN" ]; then
	echo "Building samples"
	CMAKE_OPTIONS="$CMAKE_OPTIONS $CMAKE_SAMPLES"
else
	echo "Not building samples"
fi

echo1 "Downloading OpenCV..."
git clone --depth=1 https://github.com/Itseez/opencv.git
echo1 "Downloading OpenCV extra modules..."
git clone --depth=1 https://github.com/Itseez/opencv_contrib.git
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
echo1 "Installing OpenCV..."
sudo make install
