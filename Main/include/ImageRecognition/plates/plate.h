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
   * @brief Rotate and cut the input image such that the plate is centered and
   * without border .
   * @param in Input Image
   * @param out Output Image
   */
  virtual void crop(cv::Mat const& in, cv::Mat& out) const = 0;
  /**
   * @brief Turns all pixel black, that are outside of the area of interest .
   * @param in
   * @param out Has the same saze as in .
   */
  virtual void mask(cv::Mat const& in, cv::Mat& out) const = 0;

  /**
   * @brief map_image_to_global Maps colony image coordinates to global
   * coordinates .
   * @param x X Image coordinate of the colony .
   * @param y Y Image coordinate of the colony .
   * @return Global coordinate of the colony .
   */
  virtual LocalColonyCoordinates mapImageToGlobal(PlateProfile const* plate, double x, double y) const = 0;

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
   * [0,360)
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
