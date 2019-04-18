#include "include/plates/round_plate.h"
#include <iostream>
#include "include/exception.h"

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
      markers_(markers),
      m1_(findM1(markers)) {}

RoundPlate::RoundPlate(const RoundPlate::OuterBorder& outer_border,
                       const RoundPlate::InnerBorder& inner_border,
                       const RoundPlate::Markers& markers, std::size_t m1,
                       cv::Point2d center, double angle, cv::Rect aabb)
    : Plate(center, angle, aabb),
      outer_border_(outer_border),
      inner_border_(inner_border),
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

  ret = ret * 180 / M_PI;
  return ret < 0 ? ret + 360 : ret;
}

RoundPlate::InnerBorder RoundPlate::innerBorder() const {
  return inner_border_;
}

std::size_t RoundPlate::m1() const { return m1_; }

RoundPlate* RoundPlate::rotated() const {
  OuterBorder new_outer_border;
  InnerBorder new_inner_border;
  Markers new_markers;

  /**
   * First We rotate the outer border with R, then calculate the offset that it
   * needs to be moved, and create translation matrix T from it. Then we rotate
   * and translate everything.
   */

  cv::Mat R = rotationMatrix();
  cv::transform(outer_border_, new_outer_border, R);
  // transform image
  cv::Rect aabb(cv::boundingRect(new_outer_border));
  cv::Point2d left_top(aabb.x, aabb.y);

  cv::Mat T = (cv::Mat_<double>(2, 3) << 1, 0, -left_top.x, 0, 1, -left_top.y);
  // transform contours
  cv::transform(outer_border_, new_outer_border, T);
  cv::transform(inner_border_, new_inner_border, R);
  cv::transform(inner_border_, new_inner_border, T);
  cv::transform(markers_, new_markers, R);
  cv::transform(markers_, new_markers, T);

  return new RoundPlate(new_outer_border, new_inner_border, new_markers, m1_,
                        center() - left_top, 0, aabb);
}

void RoundPlate::mask(const cv::Mat& in, cv::Mat& out) const {
  cv::Mat mask(in.rows, in.cols, in.type(), cv::Scalar());
  cv::drawContours(mask, std::vector<std::vector<cv::Point>>{inner_border_}, 0,
                   cv::Scalar::all(255), -1);

  out = in & mask;
}
}  // namespace c3picko
