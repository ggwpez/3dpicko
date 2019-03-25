#pragma once

#include "include/algorithms/helper.h"

namespace c3picko {
/**
 * @brief Represents a detected plate.
 */
class Plate {
 public:
  Plate() = default;
  /**
   * @brief Calculates the well positions A1 and H1, the angle and
   * rotation_matrix_
   */
  Plate(math::OuterBorder const& outerBorder,
        math::InnerBorder const& inner_border);
  Plate(math::OuterBorder const& outerBorder,
        math::InnerBorder const& inner_border, std::size_t a1, std::size_t h1,
        cv::Point2d inner_center, cv::Point2d outer_center, cv::Point2d center,
        math::UnitValue center_error, double angle, cv::Mat rotation_matrix);

  /**
   * @brief Rotates the plate by the negative current angle and rotates/cuts the
   * out cv::Mat such that the plate is centered and straight.
   */
  Plate normalized(cv::Mat const& in, cv::Mat& out) const;

  std::size_t a1() const;
  std::size_t h1() const;

  math::OuterBorder outerBorder() const;

 private:
  /**
   * @return {a1, h1} indices of the outer_border.
   * Assumes that the indices are counter clockwise and the image was not
   * mirrored.
   */
  static std::pair<std::size_t, std::size_t> findA1H1(
      const math::OuterBorder& outer_border,
      const math::InnerBorder& inner_border);
  /**
   * @brief Calculates the rotation of a quasi rectangle, only works counter
   * clockwise
   * @return Angle [0,360]°. I know that a two closed interval makes no sense,
   * but it comes from std::arg .
   */
  static double calculateRotation(const math::OuterBorder& cont, std::size_t a1,
                                  std::size_t h1);

  /**
   * @brief Outer border of the plate, approximated as rectangle
   */
  math::OuterBorder outer_border_;
  /**
   * @brief Inner border of the plate, approximated as hexagon
   */
  math::InnerBorder inner_border_;

  /**
   * @brief A1 well index in outer_center_
   */
  std::size_t a1_;
  /**
   * @brief H1 well index in outer_center_
   */
  std::size_t h1_;

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

  /**
   * @brief Center of the inner_border_
   */
  cv::Point2d inner_center_;
  /**
   * @brief Center of the outer_border_
   */
  cv::Point2d outer_center_;
  /**
   * @brief Average of inner_center_ and outer_center_
   */
  cv::Point2d center_;

  /**
   * @brief Relative error between the inner and outer center, |delta_x| +
   * |delta_y|
   */
  math::UnitValue center_error_;

  cv::Mat rotation_matrix_;
};
}  // namespace c3picko
