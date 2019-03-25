#include "include/plate.h"
#include <QDebug>
#include <opencv2/imgproc.hpp>

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

  std::tie(a1_, h1_) = findA1H1(outer_border_, inner_border_);
  angle_ = calculateRotation(outer_border_, a1_, h1_);
  rotation_matrix_ = cv::getRotationMatrix2D(center_, angle_, 1);

  qDebug() << "Plate Angle" << angle_;
}

Plate::Plate(const math::OuterBorder& outer_border,
             const math::InnerBorder& inner_border, std::size_t a1,
             std::size_t h1, cv::Point2d inner_center, cv::Point2d outer_center,
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
  double new_angle = calculateRotation(new_outer_border, a1_, h1_);
  // if (std::abs(new_angle) > .01 ^ std::abs(new_angle - 360) > .01)
  // qCritical() << "NewAngle was" << new_angle;

  // TODO is center_error_ still correct?
  return Plate(new_outer_border, new_inner_border, a1_, h1_,
               inner_center_ - left_top, outer_center_ - left_top,
               center_ - left_top, center_error_, 0,
               cv::Mat::eye(2, 3, CV_32F));
}

double Plate::calculateRotation(const math::OuterBorder& cont, std::size_t a1,
                                std::size_t h1) {
  // There sure is an easier way but for now:
  // Convert all angles to polarcoordinates with magnitude = 1, average them and
  // take the resulting phase angle

  auto c1 = std::polar(1., std::atan2(cont[(a1 + 1) % 4].y - cont[a1].y,
                                      cont[(a1 + 1) % 4].x - cont[a1].x));

  double ret = std::arg(c1) / M_PI * 180;
  return ret < 0 ? ret + 360 : ret;
  // FIXME
  /*auto c2 = std::polar(1., std::atan2(cont[(a1 + 2) % 4].y - cont[(a1 + 1) %
  4].y, cont[(a1 + 2) % 4].x - cont[(a1 + 1) % 4].x)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   + M_PI_2);
  auto c3
                                                                                                                                  =
  std::polar(1., std::atan2(cont[(a1 + 3) % 4].y - cont[(a1 + 2) % 4].y,
  cont[(a1 + 3) % 4].x - cont[(a1 + 2) % 4].x) + M_PI); auto c4 = std::polar(1.,
  std::atan2(cont[(a1 + 4) % 4].y - cont[(a1 + 3) % 4].y, cont[(a1 + 4) % 4].x -
  cont[(a1 + 3) % 4].x)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   - M_PI_2);

  auto sum = (c1 + c2 + c3 + c4);

  double ret = std::arg(sum) / M_PI * 180;
  return ret < 0 ? ret + 360 : ret;*/
}

std::pair<std::size_t, std::size_t> Plate::findA1H1(
    const math::OuterBorder& outer_border,
    const math::InnerBorder& inner_border) {
  // index of the outer_border points
  int ia1 = -1, ih1 = -1;
  // the length of the two diagonal edges
  double ia1_min = INFINITY, ih1_min = INFINITY;
  double eps_diag_edge_max = 10;

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
  if (ia1 == -1 || ih1 == -1 || ia1 == ih1)
    throw std::runtime_error("Could not detect diagonal edges");

  double a1_min = INFINITY, h1_min = INFINITY;
  int a1 = -1, h1 = -1;
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

  if (a1 == -1 || h1 == -1 || a1 == h1)
    throw std::runtime_error("Could not detect orientation of plate");

  return {a1, h1};
}

std::size_t Plate::a1() const { return a1_; }

std::size_t Plate::h1() const { return h1_; }

math::OuterBorder Plate::outerBorder() const { return outer_border_; }
}  // namespace c3picko
