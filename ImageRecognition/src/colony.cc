#include "include/colony.hpp"
#include "include/colony_type.h"

namespace c3picko {
template <> QJsonObject Marshalling::toJson(const Colony &value) {
  QJsonObject obj;

  obj["x"] = value.x();
  obj["y"] = value.y();
  obj["area"] = value.area();
  obj["circumference"] = value.circumference();
  obj["major_length"] = value.major_length();
  obj["brightness"] = value.brightness();
  obj["id"] = value.id();
  obj["type"] = toJson(value.type());

  return obj;
}

template <> Colony Marshalling::fromJson(const QJsonObject &obj) {
  return Colony(obj["x"].toDouble(), obj["y"].toDouble(),
                obj["area"].toDouble(), obj["circumference"].toDouble(),
                obj["major_length"].toDouble(), obj["brightness"].toDouble(),
                obj["id"].toInt(),
                fromJson<Colony::Type>(obj["type"].toObject()));
}

Colony::Colony(double x, double y, double area, double circumference,
               double major_length, double brightness, Colony::ID id, Type type)
    : x_(x), y_(y), area_(area), circumference_(circumference),
      major_length_(major_length), brightness_(brightness), id_(id),
      type_(type) {}

double Colony::x() const { return x_; }

double Colony::y() const { return y_; }

double Colony::area() const { return area_; }

double Colony::circumference() const { return circumference_; }

double Colony::major_length() const { return major_length_; }

Colony::ID Colony::id() const { return id_; }

Colony::Type Colony::type() const { return type_; }

void Colony::setType(const Colony::Type &type) { type_ = type; }

double Colony::brightness() const { return brightness_; }

} // namespace c3picko
