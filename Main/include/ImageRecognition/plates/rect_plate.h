/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include "ImageRecognition/plates/plate.h"
#include "Main/marshalling.h"

namespace d3picko {
/**
 * @brief Represents a rectangular plate.
 */
class RectPlate : public Plate {
 public:
  typedef std::array<cv::Point, 4> OuterBorder;
  typedef std::array<cv::Point, 6> InnerBorder;

  RectPlate() = default;
  /**
   * @brief Calculates the well positions A1 and H1, the angle and
   * rotation_matrix_
   */
  RectPlate(OuterBorder const& outer_border, InnerBorder const& innerBorder);
  RectPlate(OuterBorder const& outer_border, InnerBorder const& innerBorder,
			std::size_t a1, std::size_t h1, cv::Point2d inner_center,
			cv::Point2d outer_center, cv::Point2d center,
			math::UnitValue center_error, double angle, cv::Rect aabb);

  virtual void mask(cv::Mat const& in, cv::Mat& out) const override;
  virtual LocalColonyCoordinates mapImageToGlobal(PlateProfile const* plate,
												  double x,
												  double y) const override;
  virtual void crop(cv::Mat const& in, cv::Mat& out) const;

  std::size_t a1() const;
  std::size_t h1() const;

  OuterBorder const& outerBorder() const;
  InnerBorder const& innerBorder() const;

 private:
  /**
   * @return {a1, h1} indices of the outer_border.
   * Assumes that the indices are counter clockwise and the image was not
   * mirrored.
   */
  static std::pair<std::size_t, std::size_t> findA1H1(
	  const OuterBorder& outer_border, const InnerBorder& innerBorder);
  void findAndSetA1H1();
  /**
   * @brief Calculates the rotation of a quasi rectangle, only works counter
   * clockwise
   * @return Angle [0,360]°. I know that a two closed interval makes no sense,
   * but it comes from std::arg .
   */
  static double calculateRotation(const OuterBorder& cont, std::size_t a1,
								  std::size_t h1);

  /**
   * @brief Outer border of the plate, approximated as rectangle
   */
  OuterBorder outer_border_;
  /**
   * @brief Inner border of the plate, approximated as hexagon
   */
  InnerBorder inner_border_;

  /**
   * @brief inner_border_bb_ AABB of the inner border .
   * Used for quickly calculating Safet Margin collision .
   */
  cv::Rect const inner_border_aabb_;

  /**
   * @brief A1 well index in outer_center_
   */
  std::size_t a1_;
  /**
   * @brief H1 well index in outer_center_
   */
  std::size_t h1_;

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
   * @brief Relative error between the inner and outer center, |delta_x| +
   * |delta_y|
   */
  math::UnitValue center_error_;
};
MAKE_MARSHALLABLE(RectPlate*);
}  // namespace d3picko
