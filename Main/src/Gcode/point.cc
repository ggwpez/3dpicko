/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 flopicko flobotic@protonmail.com
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Gcode/point.h"

namespace d3picko {

Point::Point(float x_coordinate, float y_coordinate, float z_coordinate)
	: x_coordinate_(x_coordinate),
	  y_coordinate_(y_coordinate),
	  z_coordinate_(z_coordinate) {}

float Point::xCoordinate() const { return x_coordinate_; }

float Point::yCoordinate() const { return y_coordinate_; }

float Point::zCoordinate() const { return z_coordinate_; }
}  // namespace d3picko
