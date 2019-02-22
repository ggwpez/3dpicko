#include "include/octo_profile.h"

namespace c3picko {
namespace pi {
OctoProfile::OctoProfile(QString address, quint16 port, const ApiKey &key)
	: address_(address), port_(port), key_(key) {}

QString OctoProfile::address() const { return address_; }

quint16 OctoProfile::port() const { return port_; }

ApiKey OctoProfile::key() const { return key_; }
}  // namespace pi

template <>
QJsonObject Marshalling::toJson(const pi::OctoProfile &value) {
  QJsonObject obj;

  obj["address"] = value.address();
  obj["port"] = value.port();
  obj["key"] = Marshalling::toJson(value.key());

  return obj;
}

template <>
pi::OctoProfile Marshalling::fromJson(const QJsonObject &obj) {
  return pi::OctoProfile(
	  obj["address"].toString(), quint16(obj["port"].toInt()),
	  Marshalling::fromJson<pi::ApiKey>(obj["key"].toObject()));
}
}  // namespace c3picko
