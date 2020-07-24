#!/bin/bash
set -e

apt update -qq
apt -y install qt5-default libqt5websockets5-dev g++ build-essential pkg-config zlib1g-dev

exit 0