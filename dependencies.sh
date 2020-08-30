#!/bin/sh

# SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
#
# SPDX-License-Identifier: GPL-3.0-only

set -e

sudo apt update -qq
sudo apt -y install qt5-default libqt5websockets5-dev g++ build-essential pkg-config zlib1g-dev

exit 0