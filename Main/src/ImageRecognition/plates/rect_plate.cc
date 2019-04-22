#include "ImageRecognition/plates/rect_plate.h"

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
      inner_border_(inner_border) {
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
      a1_(a1),
      h1_(h1),
      inner_center_(inner_center),
      outer_center_(outer_center),
      center_error_(center_error) {}

double RectPlate::calculateRotation(const OuterBorder& cont, std::size_t a1,
                                    std::size_t h1) {
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
  int ia1 = -1, ih1 = -1;
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
  if (ia1 == -1 || ih1 == -1 || ia1 == ih1)
    throw std::runtime_error("Could not detect diagonal edges");

  double a1_min = std::numeric_limits<double>::max(),
         h1_min = std::numeric_limits<double>::max();
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

void RectPlate::findAndSetA1H1() {
  std::tie(a1_, h1_) = findA1H1(outer_border_, inner_border_);
}

RectPlate* RectPlate::rotated() const {
  OuterBorder new_outer_border;
  InnerBorder new_inner_border;

  // transform contours
  cv::transform(outer_border_, new_outer_border, rotationMatrix());
  cv::transform(inner_border_, new_inner_border, rotationMatrix());

  cv::Rect aabb = cv::boundingRect(new_outer_border);
  cv::Point2d left_top(aabb.x, aabb.y);

  // TODO is center_error_ still correct?
  return new RectPlate(new_outer_border, new_inner_border, a1_, h1_,
                       inner_center_ - left_top, outer_center_ - left_top,
                       center_ - left_top, center_error_, 0, aabb);
}

void RectPlate::mask(const cv::Mat& in, cv::Mat& out) const {
  cv::Mat mask(in.rows, in.cols, in.type(), cv::Scalar());
  cv::drawContours(mask,
                   std::vector<std::vector<cv::Point>>{
                       {inner_border_.begin(), inner_border_.end()}},
                   0, cv::Scalar::all(255), -1);

  out = in & mask;
}
}  // namespace c3picko
