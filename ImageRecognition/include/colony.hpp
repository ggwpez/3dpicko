#pragma once

#include "include/global.h"
#include "include/marshalling.hpp"

namespace c3picko {

class Colony {
public:
  enum class Type;
  typedef qint32 ID;

  Colony(double x, double y, double area, double circumference,
         double major_length, double brightness, ID id, Type type);

  double x() const;
  double y() const;
  double area() const;
  double circumference() const;
  double major_length() const;
  double brightness() const;
  ID id() const;

  Type type() const;
  void setType(const Type &type);

private:
  /**
   * @brief Center X position relative to the image size. Value in interval
   * [0,1]
   */
  const double x_;
  /**
   * @brief Center Y position relative to the image size. Value in interval
   * [0,1]
   */
  const double y_;

  /**
   * @brief Estimated area in pixel
   */
  const double area_;

  /**
   * @brief Estimated circumference in pixel widths
   */
  const double circumference_;

  /**
   * @brief Length of the major axis when laying a minimum area ellipse over the
   * colony relative to the image size. Value in interval [0,1].
   *
   * Use this as size when drawing bounding cirles/rectangles!
   */
  const double major_length_;

  /**
   * @brief Absolute detected brightness of the colony. Value in intervall [0,1]
   */
  const double brightness_;

  const ID id_;

  Type type_;
};

MAKE_MARSHALLABLE(Colony);
} // namespace c3picko
