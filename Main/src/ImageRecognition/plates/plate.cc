/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "ImageRecognition/plates/plate.h"

#include <QDebug>
#include <opencv2/imgproc.hpp>

#include "ImageRecognition/plates/rect_plate.h"
#include "ImageRecognition/plates/round_plate.h"

namespace d3picko {
Plate::Plate(cv::Point2d center, double angle, cv::Rect bb)
	: center_(center),
	  angle_(angle),
	  aabb_(bb),
	  rotation_matrix_(cv::getRotationMatrix2D(cv::Point2f(), angle_, 1)) {}

cv::Point2d Plate::center() const { return center_; }

cv::Mat const& Plate::rotationMatrix() const { return rotation_matrix_; }

double Plate::angle() const { return angle_; }

cv::Rect Plate::aabb() const { return aabb_; }

template <>
QJsonObject Marshalling::toJson(std::unique_ptr<Plate> const& value) {
  return Marshalling::toJson(value.get());
}

template <>
std::unique_ptr<Plate> Marshalling::fromJson(const QJsonObject& obj) {
  return std::unique_ptr<Plate>(Marshalling::fromJson<Plate*>(obj));
}

template <>
QJsonObject Marshalling::toJson(Plate* const& value) {
  QJsonObject obj;
  RoundPlate* round;
  RectPlate* rect;

  if ((round = dynamic_cast<RoundPlate*>(value))) {
	obj["type"] = "round";
	obj["data"] = Marshalling::toJson(round);
  } else if ((rect = dynamic_cast<RectPlate*>(value))) {
	obj["type"] = "rect";
	obj["data"] = Marshalling::toJson(rect);
  } else
	Q_UNREACHABLE();

  return obj;
}

template <>
Plate* Marshalling::fromJson(const QJsonObject& obj) {
  if (obj["type"] == "round")
	return Marshalling::fromJson<RoundPlate*>(obj["data"]);
  else if (obj["type"] == "rect")
	return Marshalling::fromJson<RectPlate*>(obj["data"]);
  else
	Q_UNREACHABLE();
}
}  // namespace d3picko
