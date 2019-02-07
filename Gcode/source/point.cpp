#include "include/point.h"

namespace c3picko {

	Point::Point(const QJsonObject& obj)
		: JsonConstructable (obj),
		  x_coordinate_(obj["x_coordinate"].toDouble()),
		  y_coordinate_(obj["y_coordinate"].toDouble()),
		  z_coordinate_(obj["z_coordinate"].toDouble())
	{

	}

	Point::Point(float x_coordinate, float y_coordinate, float z_coordinate)
	: JsonConstructable (QJsonObject()),
	  x_coordinate_(x_coordinate),
	  y_coordinate_(y_coordinate),
	  z_coordinate_(z_coordinate) {}

float Point::xCoordinate() const { return x_coordinate_; }

float Point::yCoordinate() const { return y_coordinate_; }

float Point::zCoordinate() const { return z_coordinate_; }

void Point::write(QJsonObject& obj) const
{
	obj["x_coordinate"] = x_coordinate_;
	obj["y_coordinate"] = y_coordinate_;
	obj["z_coordinate"] = z_coordinate_;
}
}  // namespace c3picko
