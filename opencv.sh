######################################
# INSTALL OPENCV ON UBUNTU OR DEBIAN #
######################################

# |          THIS SCRIPT IS TESTED CORRECTLY ON          |
# |------------------------------------------------------|
# | OS               | OpenCV       | Test | Last test   |
# |------------------|--------------|------|-------------|
# | Ubuntu 20.04 LTS | OpenCV 3.4.10| OK   | 24 Jul 2020 |
# | Ubuntu 18.04 LTS | OpenCV 3.4.2 | OK   | 18 Jul 2018 |
# | Debian 9.5       | OpenCV 3.4.2 | OK   | 18 Jul 2018 |
# |----------------------------------------------------- |
# | Debian 9.0       | OpenCV 3.2.0 | OK   | 25 Jun 2017 |
# | Debian 8.8       | OpenCV 3.2.0 | OK   | 20 May 2017 |
# | Ubuntu 16.04 LTS | OpenCV 3.2.0 | OK   | 20 May 2017 |

# Save path, so we can restore it

set -e
gwd=$(pwd)
cd ~

# VERSION TO BE INSTALLED

OPENCV_VERSION='3.4.10'

if command -v COMMAND &> /dev/null; then
	opencv_version=`pkg-config --modversion opencv | sed "s/.*-\(.*\)\.[a-zA-Z0-9]\{3\}$/\1/"`
fi

if [ -z "$opencv_version" ]; then
    echo "OpenCV is NOT installed"
else
	echo "OpenCV is installed"
	exit 0
fi

# 1. KEEP UBUNTU OR DEBIAN UP TO DATE

# 2. INSTALL THE DEPENDENCIES

# Build tools:
sudo apt-get install -y cmake zlib1g-dev libjpeg-dev libwebp-dev libpng-dev libtiff5-dev libopenexr-dev libgdal-dev libtbb-dev unzip wget

# 3. INSTALL THE LIBRARY

wget -q https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip -O opencv-${OPENCV_VERSION}.zip
unzip -qq opencv-${OPENCV_VERSION}.zip
rm opencv-${OPENCV_VERSION}.zip
mv opencv-${OPENCV_VERSION} OpenCV
cd OpenCV
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release -DBUILD_LIST=core,imgproc,highgui,photo,calib3d ..
make -j$(nproc) 2> /dev/null
sudo make install
sudo ldconfig

# 4. EXECUTE SOME OPENCV EXAMPLES AND COMPILE A DEMONSTRATION

# To complete this step, please visit 'http://milq.github.io/install-opencv-ubuntu-debian'.

# Restort path
cd "$gwd"
