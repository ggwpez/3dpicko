#include "include/plate_result.h"
#include <opencv2/imgproc.hpp>
#include <tuple>

namespace c3picko {
Plate::Plate(const math::OuterBorder& outer_border,
             const math::InnerBorder& inner_border)
    : outer_border_(outer_border),
      inner_border_(inner_border),
      inner_center_(math::gravityCenter(inner_border_)),
      outer_center_(math::gravityCenter(outer_border_)),
      center_((outer_center_ + inner_center_) / 2) {
  center_error_ = std::abs(1 - outer_center_.x / inner_center_.x) +
                  std::abs(1 - outer_center_.y / inner_center_.y);

  std::tie(a1_, h1_) = math::findA1(outer_border_, inner_border_);
  angle_ = math::calculateOuterRotation(outer_border_, a1_, h1_);
  rotation_matrix_ = cv::getRotationMatrix2D(center_, angle_, 1);

  qDebug() << "Angle" << angle_;
}

Plate::Plate(const math::OuterBorder& outer_border,
             const math::InnerBorder& inner_border, int a1, int h1,
             cv::Point2d inner_center, cv::Point2d outer_center,
             cv::Point2d center, math::UnitValue center_error, double angle,
             cv::Mat rotation_matrix)
    : outer_border_(outer_border),
      inner_border_(inner_border),
      a1_(a1),
      h1_(h1),
      inner_center_(inner_center),
      outer_center_(outer_center),
      center_(center),
      center_error_(center_error),
      angle_(angle),
      rotation_matrix_(rotation_matrix) {}

Plate Plate::normalized(const cv::Mat& in, cv::Mat& out) const {
  if (angle_ == 0)  // ok
    return *this;

  math::OuterBorder new_outer_border;
  math::InnerBorder new_inner_border;

  // transform contours
  cv::transform(outer_border_, new_outer_border, rotation_matrix_);
  cv::transform(inner_border_, new_inner_border, rotation_matrix_);

  // transform image
  cv::Rect aabb = cv::boundingRect(new_outer_border);
  cv::Point2d left_top(aabb.x, aabb.y);
  cv::warpAffine(in, out, rotation_matrix_,
                 cv::Size(in.cols, in.rows));  // TODO try cv::transform
  out = out(aabb);

  // After rotation, the angle of the plate should be nealy zero
  double new_angle = math::calculateOuterRotation(new_outer_border, a1_, h1_);
  // if (std::abs(new_angle) > .01 ^ std::abs(new_angle - 360) > .01)
  // qCritical() << "NewAngle was" << new_angle;

  // TODO is center_error_ still correct?
  return Plate(new_outer_border, new_inner_border, a1_, h1_,
               inner_center_ - left_top, outer_center_ - left_top,
               center_ - left_top, center_error_, 0,
               cv::Mat::eye(2, 3, CV_32F));
}

int Plate::a1() const { return a1_; }

int Plate::h1() const { return h1_; }

math::OuterBorder Plate::outerBorder() const { return outer_border_; }
}  // namespace c3picko
