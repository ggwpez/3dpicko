#pragma once

#include "ImageRecognition/plates/plate.h"
#include "Main/marshalling.h"

namespace c3picko {
/**
 * @brief Represents a rectangular plate.
 */
class RoundPlate : public Plate {
 public:
  typedef std::vector<cv::Point> OuterBorder;
  typedef std::vector<cv::Point> InnerBorder;
  typedef std::array<cv::Point, 3> Markers;

  RoundPlate() = default;
  /**
   * @brief Calculates the well positions A1 and H1, the angle and
   * rotation_matrix_
   */
  RoundPlate(OuterBorder const& outer_border, InnerBorder const& innerBorder,
			 Markers const& markers);
  RoundPlate(OuterBorder const& outer_border, InnerBorder const& innerBorder,
			 Markers const& markers, std::size_t m1, cv::Point2d center,
			 double angle, cv::Rect aabb);

  virtual RoundPlate* rotated() const override;
  virtual void mask(cv::Mat const& in, cv::Mat& out) const override;

  std::size_t m1() const;

  InnerBorder const& innerBorder() const;
  InnerBorder const& outerBorder() const;
  Markers const& markers() const;

 private:
  static std::size_t findM1(const Markers& markers);
  /**
   * @brief Calculates the angle defined by the markers
   * @return [0,360]
   */
  static double calculateRotation(Markers const& markers,
								  std::size_t start_marker, cv::Point center);

  /**
   * @brief Outer border of the plate, approximated as rectangle
   */
  OuterBorder const outer_border_;

  /**
   * @brief Inner border of the plate, approximated as hexagon
   */
  InnerBorder const inner_border_;

  /**
   * @brief The three distinct corners from the round plate .
   */
  Markers const markers_;

  /**
   * @brief Index of the primary marker in the markers_ array .
   */
  std::size_t const m1_;
};
MAKE_MARSHALLABLE(RoundPlate*);
}  // namespace c3picko
