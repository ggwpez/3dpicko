#ifndef POINT_H
#define POINT_H

#include "include/json_constructable.hpp"

namespace c3picko {

/**
 * @file point.h
 * @author flopicko
 * @brief Class representing a simple 3D point.
 *
 */
class Point : public JsonConstructable{
 public:
	Point(QJsonObject const&);
  /**
   * @brief Point
   * @param x_coordinate
   * @param y_coordinate
   * @param z_coordinate optional, a default value is set so that if
   * the z coordinate is not needed it does not need to be set
   */
  Point(float x_coordinate, float y_coordinate, float z_coordinate = 0);

  float xCoordinate() const;
  float yCoordinate() const;
  float zCoordinate() const;

  void write(QJsonObject& obj) const;

 private:
  const float x_coordinate_;
  const float y_coordinate_;
  const float z_coordinate_;
};

using LocalColonyCoordinates = Point;
using LocalMasterCoordinates = Point;
using LocalWellCoordinates = Point;
using GlobalColonyCoordinates = Point;
using GlobalMasterCoordinates = Point;
using GlobalWellCoordinates = Point;
}  // namespace c3picko
#endif  // POINT_H
