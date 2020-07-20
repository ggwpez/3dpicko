#include "ImageRecognition/plates/rect_plate.h"

#include <QJsonArray>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "Gcode/plateprofile.h"

namespace c3picko {
// TODO im not proud
RectPlate::RectPlate(const OuterBorder& outer_border,
					 const InnerBorder& inner_border)
	: Plate((math::gravityCenter(outer_border) +
			 math::gravityCenter(inner_border)) /
				2,
			calculateRotation(
				outer_border, a1_ = findA1H1(outer_border, inner_border).first,
				h1_ = findA1H1(outer_border, inner_border).second),
			cv::boundingRect(outer_border)),
	  outer_border_(outer_border),
	  inner_border_(inner_border),
	  inner_border_aabb_(cv::boundingRect(inner_border)),
	  inner_center_(math::gravityCenter(inner_border)),
	  outer_center_(math::gravityCenter(outer_border)) {
  center_error_ = std::abs(1 - outer_center_.x / inner_center_.x) +
				  std::abs(1 - outer_center_.y / inner_center_.y);
}

RectPlate::RectPlate(const OuterBorder& outer_border,
					 const InnerBorder& inner_border, std::size_t a1,
					 std::size_t h1, cv::Point2d inner_center,
					 cv::Point2d outer_center, cv::Point2d center,
					 math::UnitValue center_error, double angle, cv::Rect aabb)
	: Plate(center, angle, aabb),
	  outer_border_(outer_border),
	  inner_border_(inner_border),
	  inner_border_aabb_(cv::boundingRect(inner_border)),
	  a1_(a1),
	  h1_(h1),
	  inner_center_(inner_center),
	  outer_center_(outer_center),
	  center_error_(center_error) {}

double RectPlate::calculateRotation(const OuterBorder& cont, std::size_t a1,
									std::size_t) {
  cv::Point p1 = cont[(a1 + 1) % 4] - cont[a1];
  double v1 = std::atan2(p1.y, p1.x);

  cv::Point p2 = cont[(a1 + 2) % 4] - cont[(a1 + 1) % 4];
  double v2 = std::atan2(p2.y, p2.x) + M_PI / 2;

  cv::Point p3 = cont[(a1 + 3) % 4] - cont[(a1 + 2) % 4];
  double v3 = std::atan2(p3.y, p3.x) + M_PI;

  cv::Point p4 = cont[(a1 + 4) % 4] - cont[(a1 + 3) % 4];
  double v4 = std::atan2(p4.y, p4.x) - M_PI / 2;

  double ret =
	  std::atan2(
		  (std::sin(v1) + std::sin(v2) + std::sin(v3) + std::sin(v4)) / 4,
		  (std::cos(v1) + std::cos(v2) + std::cos(v3) + std::cos(v4)) / 4) /
	  M_PI * 180;
  return ret < 0 ? ret + 360 : ret;
}

std::pair<std::size_t, std::size_t> RectPlate::findA1H1(
	const OuterBorder& outer_border, const InnerBorder& inner_border) {
  // index of the outer_border points
  std::size_t ia1 = 100, ih1 = 100;
  // the length of the two diagonal edges
  double ia1_min = std::numeric_limits<double>::max(),
		 ih1_min = std::numeric_limits<double>::max();

  for (std::size_t i = 0; i < inner_border.size(); ++i)
	for (std::size_t j = i + 1; j < inner_border.size(); ++j) {
	  double dist = cv::norm(inner_border[i] - inner_border[j]);

	  if (dist < ia1_min) {
		ia1_min = dist;
		ia1 = i;
	  }
	}
  for (std::size_t i = 0; i < inner_border.size(); ++i)
	for (std::size_t j = i + 1; j < inner_border.size(); ++j) {
	  double dist = cv::norm(inner_border[i] - inner_border[j]);

	  if (dist < ih1_min && i != ia1) {
		ih1_min = dist;
		ih1 = i;
	  }
	}

  // NOTE Try to increase eps
  if (ia1 == 100 || ih1 == 100 || ia1 == ih1)
	throw std::runtime_error("Could not detect diagonal edges");

  double a1_min = std::numeric_limits<double>::max(),
		 h1_min = std::numeric_limits<double>::max();
  std::size_t a1 = 100, h1 = 100;
  for (std::size_t i = 0; i < outer_border.size(); ++i) {
	double dist_a1 = cv::norm(outer_border[i] - inner_border[ia1]);
	double dist_h1 = cv::norm(outer_border[i] - inner_border[ih1]);

	if (dist_a1 < a1_min) {
	  a1_min = dist_a1;
	  a1 = i;
	}
	if (dist_h1 < h1_min) {
	  h1_min = dist_h1;
	  h1 = i;
	}
  }

  // swap a1 and h1 such that h1 is counter clockwise of a1
  if ((h1 + 1) % outer_border.size() != a1) std::swap(a1, h1);
  Q_ASSERT((h1 + 1) % outer_border.size() == a1);

  if (a1 == 100 || h1 == 100 || a1 == h1)
	throw std::runtime_error("Could not detect orientation of plate");

  return {a1, h1};
}

void RectPlate::findAndSetA1H1() {
  std::tie(a1_, h1_) = findA1H1(outer_border_, inner_border_);
}

void RectPlate::mask(const cv::Mat& in, cv::Mat& out) const {
  cv::Mat mask(in.rows, in.cols, in.type(), cv::Scalar());
  int const bw = in.cols * 0.05, bh = in.rows * 0.08;  // percent margin
  int const w = in.cols - bw, h = in.rows - bh;
  std::vector<std::vector<cv::Point>> cts{
	  {{cv::Point(bw, bh), cv::Point(w, bh), cv::Point(w, h),
		cv::Point(bw, h)}}};
  cv::drawContours(mask, cts, 0, cv::Scalar::all(255), -1);

  out = in & mask;
}

LocalColonyCoordinates RectPlate::mapImageToGlobal(const PlateProfile* plate,
												   double x, double y) const {
  return {float(x * plate->redFrameWidth()),
		  float((1.0 - y) * plate->redFrameHeight())};
}

void RectPlate::crop(const cv::Mat& in, cv::Mat& out) const {
  float h = cv::norm(outer_border_[h1_] - outer_border_[a1_]);
  float w = cv::norm(outer_border_[(h1_ + 2) % 4] - outer_border_[a1_]);
  std::array<cv::Point2f, 4> border{
	  {outer_border_[a1_], outer_border_[(a1_ + 1) % 4],
	   outer_border_[(a1_ + 2) % 4], outer_border_[(a1_ + 3) % 4]}};
  std::array<cv::Point2f, 4> pts = {{{0, 0}, {w, 0}, {w, h}, {0, h}}};

  cv::Mat T = cv::findHomography(border, pts);
  cv::warpPerspective(in, out, T, cv::Size(w, h));
}

const RectPlate::InnerBorder& RectPlate::innerBorder() const {
  return inner_border_;
}

const RectPlate::OuterBorder& RectPlate::outerBorder() const {
  return outer_border_;
}

template <>
QJsonObject Marshalling::toJson(RectPlate* const& value) {
  QJsonObject obj;
  QJsonArray inner, outer;

  for (auto const& e : value->innerBorder())
	inner.push_back(Marshalling::toJson(e));
  for (auto const& e : value->outerBorder())
	outer.push_back(Marshalling::toJson(e));

  obj["inner"] = inner;
  obj["outer"] = outer;

  return obj;
}

template <>
RectPlate* Marshalling::fromJson(const QJsonObject& obj) {
  RectPlate::InnerBorder inner;
  RectPlate::OuterBorder outer;
  QJsonArray jinner(obj["inner"].toArray()), jouter(obj["outer"].toArray());

  for (std::size_t i = 0; i < inner.size(); ++i)
	inner[i] = Marshalling::fromJson<cv::Point>(jinner[i]);
  for (std::size_t i = 0; i < outer.size(); ++i)
	outer[i] = Marshalling::fromJson<cv::Point>(jouter[i]);

  return new RectPlate(outer, inner);
}
}  // namespace c3picko
