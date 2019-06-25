#include "ImageRecognition/colony.h"

namespace c3picko {
template <>
QJsonObject Marshalling::toJson(const Colony& value) {
  QJsonObject obj;

  obj["x"] = value.x();
  obj["y"] = value.y();
  obj["area"] = value.area();
  obj["circumference"] = value.circumference();
  obj["major_length"] = value.major_length();
  obj["brightness"] = value.brightness();
  obj["id"] = value.id();
  obj["excluded_by"] = value.excludedBy();

  return obj;
}

template <>
Colony Marshalling::fromJson(const QJsonObject& obj) {
  return Colony(obj["x"].toDouble(), obj["y"].toDouble(),
				obj["area"].toDouble(), obj["circumference"].toDouble(),
				obj["major_length"].toDouble(), obj["brightness"].toDouble(),
				obj["id"].toInt(),
				obj["excluded_by"].toString());  // no HTML-escape needed
}

Colony::Colony(double x, double y, double area, double circumference,
			   double major_length, double brightness, Colony::ID id,
			   AlgoSetting::ID excluded_by)
	: x_(x),
	  y_(y),
	  area_(area),
	  circumference_(circumference),
	  major_length_(major_length),
	  brightness_(brightness),
	  id_(id),
	  excluded_by_(excluded_by) {}

double Colony::x() const { return x_; }

double Colony::y() const { return y_; }

double Colony::area() const { return area_; }

double Colony::circumference() const { return circumference_; }

double Colony::major_length() const { return major_length_; }

Colony::ID Colony::id() const { return id_; }

bool Colony::included() const { return excluded_by_.isEmpty(); }

AlgoSetting::ID Colony::excludedBy() const { return excluded_by_; }

void Colony::setExcludedBy(const AlgoSetting::ID& excluded_by) {
  excluded_by_ = excluded_by;
}

double Colony::brightness() const { return brightness_; }

}  // namespace c3picko
