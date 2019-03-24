#pragma once

#include "include/algorithms/helper.h"

namespace c3picko {
class Plate {
 public:
  Plate() = default;
  Plate(math::OuterBorder const &outerBorder,
        math::InnerBorder const &inner_border);
  Plate(math::OuterBorder const &outerBorder,
        math::InnerBorder const &inner_border, int a1, int h1,
        cv::Point2d inner_center, cv::Point2d outer_center, cv::Point2d center,
        math::UnitValue center_error, double angle, cv::Mat rotation_matrix);

  /**
   * @brief Rotates the plate by the negative current angle and rotates/cuts the
   * out cv::Mat such that the plate is centered and straight.
   */
  Plate normalized(cv::Mat const &in, cv::Mat &out) const;

  int a1() const;

  int h1() const;

  math::OuterBorder outerBorder() const;

 private:
  math::OuterBorder outer_border_;
  math::InnerBorder inner_border_;
  int a1_, h1_;

  /**
   * The unweighted average of all outer borders' angle.
   * Complex angle, 0 degree is from left to right aka (1,0).
   *
   * TODO weight?
   */
  double angle_;

  /**
   * @brief Relative error to the optimal bb side ratio of the plate-frame.
   */
  math::UnitValue outer_bb_ratio_error;

  cv::Point2d inner_center_, outer_center_;
  cv::Point2d center_;

  /**
   * @brief Relative error between the inner and outer center, |x_error| +
   * |y_error|
   */
  math::UnitValue center_error_;

  cv::Mat rotation_matrix_;
  static std::pair<int, int> findA1(const math::OuterBorder &outer_border,
                                    const math::InnerBorder &inner_border);
  static double calculateOuterRotation(const math::OuterBorder &cont, int a1,
                                       int h1);
};
}  // namespace c3picko
