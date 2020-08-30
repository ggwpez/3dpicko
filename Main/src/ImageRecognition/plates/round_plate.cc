/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "ImageRecognition/plates/round_plate.h"

#include <QJsonArray>
#include <opencv2/calib3d/calib3d.hpp>

#include "Main/exception.h"

namespace c3picko {
RoundPlate::RoundPlate(const RoundPlate::OuterBorder& outer_border,
					   const RoundPlate::InnerBorder& inner_border,
					   const RoundPlate::Markers& markers)
	// TODO gravityCenter called twice...
	: Plate(math::gravityCenter(inner_border),
			calculateRotation(markers, findM1(markers),
							  math::gravityCenter(inner_border)),
			cv::boundingRect(outer_border)),
	  outer_border_(outer_border),
	  inner_border_(inner_border),
	  inner_border_aabb_width_2_squared_(
		  std::pow(cv::boundingRect(inner_border).width / 2, 2)),
	  markers_(markers),
	  m1_(findM1(markers)) {}

RoundPlate::RoundPlate(const RoundPlate::OuterBorder& outer_border,
					   const RoundPlate::InnerBorder& inner_border,
					   const RoundPlate::Markers& markers, std::size_t m1,
					   cv::Point2d center, double angle, cv::Rect aabb)
	: Plate(center, angle, aabb),
	  outer_border_(outer_border),
	  inner_border_(inner_border),
	  inner_border_aabb_width_2_squared_(
		  std::pow(cv::boundingRect(inner_border).width / 2, 2)),
	  markers_(markers),
	  m1_(m1) {}

std::size_t RoundPlate::findM1(const RoundPlate::Markers& markers) {
  double d01 = cv::norm(markers[0] - markers[1]),
		 d12 = cv::norm(markers[1] - markers[2]),
		 d20 = cv::norm(markers[2] - markers[0]);

  if (d01 < d12 && d01 < d20)
	return 2;
  else if (d12 < d01 && d12 < d20)
	return 0;
  else if (d20 < d01 && d20 < d12)
	return 1;
  else
	throw Exception("Could not find start marker");
}

double RoundPlate::calculateRotation(RoundPlate::Markers const& markers,
									 std::size_t start_marker,
									 cv::Point center) {
  cv::Point m1 = markers[start_marker] - center;
  double a1 = std::atan2(m1.y, m1.x);

  cv::Point m2 = markers[(start_marker + 1) % 3] - center;
  double a2 = std::atan2(m2.y, m2.x) - 2.35619449019;  // -135 °

  cv::Point m3 = markers[(start_marker + 2) % 3] - center;
  double a3 = std::atan2(m3.y, m3.x) + 2.35619449019;

  // https://en.wikipedia.org/wiki/Mean_of_circular_quantities
  double ret = std::atan2((std::sin(a1) + std::sin(a2) + std::sin(a3)) / 3,
						  (std::cos(a1) + std::cos(a2) + std::cos(a3)) / 3);

  ret = (ret - M_PI) * 180 / M_PI;	// -M_PI rotates the edgy marker to the
									// left, such as it lies in the picker.
  return ret < 0 ? ret + 360 : ret;
}

const RoundPlate::Markers& RoundPlate::markers() const { return markers_; }

const RoundPlate::InnerBorder& RoundPlate::innerBorder() const {
  return inner_border_;
}

const RoundPlate::InnerBorder& RoundPlate::outerBorder() const {
  return outer_border_;
}

std::size_t RoundPlate::m1() const { return m1_; }

void RoundPlate::mask(const cv::Mat& in, cv::Mat& out) const {
  cv::Mat mask(in.rows, in.cols, in.type(), cv::Scalar());
  cv::drawContours(mask, std::vector<std::vector<cv::Point>>{inner_border_}, 0,
				   cv::Scalar::all(255), -1);

  out = in & mask;
}

LocalColonyCoordinates RoundPlate::mapImageToGlobal(const PlateProfile* plate,
													double x, double y) const {
  return {float(x * 98 + 12.5), float((1.0 - y) * 92.5 - 2.8)};
}

void RoundPlate::crop(const cv::Mat& in, cv::Mat& out) const {
  float h = cv::norm(markers_[m1_] - markers_[(m1_ + 1) % 3]);
  float w = cv::norm(markers_[(m1_ + 2) % 4] - markers_[m1_]);
  std::array<cv::Point2f, 3> border{
	  {markers_[m1_], markers_[(m1_ + 2) % 3], markers_[(m1_ + 1) % 3]}};
  std::array<cv::Point2f, 3> pts = {{{0, h / 2}, {w, 0}, {w, h}}};

  cv::Mat T = cv::findHomography(border, pts);
  cv::warpPerspective(in, out, T, cv::Size(w, h));
}

template <>
QJsonObject Marshalling::toJson(RoundPlate* const& value) {
  QJsonObject obj;
  QJsonArray inner, outer, markers;

  for (auto const& e : value->innerBorder())
	inner.push_back(Marshalling::toJson(e));
  for (auto const& e : value->outerBorder())
	outer.push_back(Marshalling::toJson(e));
  for (auto const& e : value->markers())
	markers.push_back(Marshalling::toJson(e));

  obj["inner"] = inner;
  obj["outer"] = outer;
  obj["markers"] = markers;

  return obj;
}

template <>
RoundPlate* Marshalling::fromJson(const QJsonObject& obj) {
  RoundPlate::InnerBorder inner;
  RoundPlate::OuterBorder outer;
  RoundPlate::Markers markers;
  QJsonArray jinner(obj["inner"].toArray()), jouter(obj["outer"].toArray()),
	  jmarkers(obj["markers"].toArray());

  for (int i = 0; i < jinner.size(); ++i)
	inner.push_back(Marshalling::fromJson<cv::Point>(jinner[i]));
  for (int i = 0; i < jouter.size(); ++i)
	outer.push_back(Marshalling::fromJson<cv::Point>(jouter[i]));
  for (std::size_t i = 0; i < markers.size(); ++i)
	markers[i] = Marshalling::fromJson<cv::Point>(jmarkers[i]);

  return new RoundPlate(outer, inner, markers);
}
}  // namespace c3picko
