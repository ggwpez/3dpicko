#include "include/point.h"

namespace c3picko {

	Point::Point(const QJsonObject& obj)
		: JsonConstructable(QJsonObject()),
		  x_coordinate_(obj["x"].toDouble()),
		  y_coordinate_(obj["y"].toDouble())
	{

	}

	Point::Point(float x_coordinate, float y_coordinate)
  : JsonConstructable(QJsonObject()),
	  x_coordinate_(x_coordinate),
	y_coordinate_(y_coordinate) {

}

float Point::xCoordinate() const {
  return x_coordinate_;
}

float Point::yCoordinate() const {
	return y_coordinate_;
}

void Point::write(QJsonObject& obj) const
{
	obj["x"] = x_coordinate_;
	obj["y"] = y_coordinate_;
}
} // namespace c3picko
