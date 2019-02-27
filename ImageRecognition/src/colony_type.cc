#include "include/colony_type.h"

namespace c3picko {

template <>
QJsonObject Marshalling::toJson(const Colony::Type &value) {
  QJsonObject obj;

  obj["value"] = static_cast<int>(value);

  return obj;
}

template <>
Colony::Type Marshalling::fromJson(const QJsonObject &obj) {
  return static_cast<Colony::Type>(obj["value"].toInt());
}
}  // namespace c3picko
