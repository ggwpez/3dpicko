#ifndef POINT_H
#define POINT_H

#include "include/json_constructable.hpp"

namespace c3picko {

/**
 * @file point.h
 * @author flopicko
 * @brief Class representing a simple 2D point.
 *
 */
class Point : public JsonConstructable {
 public:
	Point(QJsonObject const&);
  Point(float x_coordinate, float y_coordinate);
  float xCoordinate() const;
  float yCoordinate() const;

  void write(QJsonObject&) const;

 private:
  const float x_coordinate_;
  const float y_coordinate_;
};

using LocalColonyCoordinates = Point;
using LocalMasterCoordinates = Point;
using LocalWellCoordinates = Point;
using GlobalColonyCoordinates = Point;
using GlobalMasterCoordinates = Point;
using GlobalWellCoordinates = Point;
} // namespace c3picko
#endif // POINT_H
