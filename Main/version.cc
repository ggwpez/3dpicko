#include "include/version.h"

namespace c3picko {
Version::Version(Version::ID id, QDateTime date) : id_(id), date_(date) {}

QString Version::id() const { return id_; }

template <>
QJsonObject Marshalling::toJson(const Version& value) {
  QJsonObject obj;

  obj["hash"] = value.id();
  obj["date"] = Marshalling::toJson(value.date());

  return obj;
}

template <>
Version Marshalling::fromJson(const QJsonObject& obj) {
  return Version(obj["hash"].toString(),
                 Marshalling::fromJson<QDateTime>(obj["date"]));
}

QDateTime Version::date() const { return date_; }

}  // namespace c3picko
