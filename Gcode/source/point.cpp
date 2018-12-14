#include "include/point.h"

namespace c3picko {

Point::Point(float x_coordinate, float y_coordinate)
  : x_coordinate_(x_coordinate),
    y_coordinate_(y_coordinate) {

}

float Point::xCoordinate() const {
  return x_coordinate_;
}

float Point::yCoordinate() const {
  return y_coordinate_;
}
} // namespace c3picko
