#pragma once

#include <memory>
#include "Gcode/point.h"
#include "ImageRecognition/algorithms/helper.h"
#include "Main/marshalling.h"

namespace c3picko {
/**
 * @brief Represents a detected plate.
 *
 * Can be used to cut an image to the clipping of the detected plate.
 */
class Plate {
 public:
  Plate(cv::Point2d center, double angle, cv::Rect aabb);
  virtual ~Plate() = default;
  /**
   * @brief Rotates the plate by the negative current angle and rotates/cuts the
   * out cv::Mat such that the plate is centered and straight.
   */
  virtual Plate* rotated() const = 0;
  /**
   * @brief Rotate and cut the input image such that the plate is centered and
   * without border .
   * @param in Input Image
   * @param out Output Image
   */
  static void crop(Plate const& original, Plate const& rotated,
				   cv::Mat const& in, cv::Mat& out);
  /**
   * @brief Turns all pixel black, that are outside of the area of interest .
   * @param in
   * @param out Has the same saze as in .
   */
  virtual void mask(cv::Mat const& in, cv::Mat& out) const = 0;

  /**
   * @brief isInsideSafetyMargin
   * @return Whether the Colony can be picked .
   */
  virtual bool isInsideSafetyMargin(cv::Point2d pos,
									math::UnitValue radius) const = 0;

  /**
   * @brief isPickable Is the colony at this pixel coordinate pickable ?
   * @param x
   * @param y
   * @return
   */
  virtual bool isPixelPickable(int x, int y) const = 0;

  /**
   * @brief map_image_to_global Maps colony image coordinates to global
   * coordinates .
   * @param x X Image coordinate of the colony .
   * @param y Y Image coordinate of the colony .
   * @return Global coordinate of the colony .
   */
  virtual LocalColonyCoordinates mapImageToGlobal(double x, double y) const = 0;

  cv::Point2d center() const;
  const cv::Mat& rotationMatrix() const;
  double angle() const;
  cv::Rect aabb() const;

 protected:
  /**
   * @brief Gravity center of the detected border, not the same as bb_.center .
   */
  cv::Point2d const center_;

  /**
   * The unweighted average of all outer borders' angle.
   * Use it to cut the image.
   * Imaginary inverted Complex angle, ex: 0° is (1,0) and 90° is (0,-1) .
   */
  double const angle_;

  /**
   * @brief Bounding box for cutting the image with angle_ .
   */
  cv::Rect const aabb_;

  cv::Mat const rotation_matrix_;
};
MAKE_MARSHALLABLE(Plate*);
MAKE_MARSHALLABLE(std::unique_ptr<Plate>);
}  // namespace c3picko
