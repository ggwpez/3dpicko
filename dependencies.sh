#!/bin/bash
set -e

sudo apt update -qq
sudo apt -y -qq install qt5-default libqt5websockets5-dev g++ libxml2-utils build-essential doxygen graphviz cppcheck clang-format pkg-config zlib1g-dev
sudo bash opencv.sh

exit 0