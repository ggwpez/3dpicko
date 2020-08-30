#!/bin/sh

# SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
#
# SPDX-License-Identifier: GPL-3.0-only

set -e
gwd=$(pwd)
cd /tmp

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

# Build tools:
sudo apt-get install -y cmake zlib1g-dev libjpeg-dev libwebp-dev libpng-dev libtiff5-dev libopenexr-dev libgdal-dev libtbb-dev unzip wget

wget -q https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip -O opencv-${OPENCV_VERSION}.zip
unzip -qq opencv-${OPENCV_VERSION}.zip
rm opencv-${OPENCV_VERSION}.zip
mv opencv-${OPENCV_VERSION} OpenCV
cd OpenCV && mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release -DBUILD_LIST=core,imgproc,highgui,photo,calib3d ..
make -j$(nproc) 2> /dev/null
# GCC likes to sigsegv, retry.
make -j$(nproc) 2> /dev/null
sudo make install
sudo ldconfig

# Restort path
cd "$gwd"
