#include "include/apikey.h"

namespace c3picko {
template <> QJsonObject Marshalling::toJson(const pi::ApiKey &value) {
  QJsonObject obj;

  obj["data"] = value;

  return obj;
}

template <> pi::ApiKey Marshalling::fromJson(const QJsonObject &obj) {
  return pi::ApiKey(obj["data"].toString());
}
} // namespace c3picko
