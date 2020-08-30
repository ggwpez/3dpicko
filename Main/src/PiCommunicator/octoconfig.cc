/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/octoconfig.h"

namespace c3picko {
namespace pi {
OctoConfig::OctoConfig(QString address, ApiKey const& key, QString protocol,
					   quint16 port)
	: address_(address), key_(key), protocol_(protocol), port_(port) {}

QString OctoConfig::address() const { return address_; }

quint16 OctoConfig::port() const { return port_; }

ApiKey OctoConfig::key() const { return key_; }

QString OctoConfig::protocol() const { return protocol_; }
}  // namespace pi

template <>
QJsonObject Marshalling::toJson(const pi::OctoConfig& value) {
  QJsonObject obj;

  obj["address"] = value.address();
  obj["api_key"] = value.key().key();
  obj["protocol"] = value.protocol();
  obj["port"] = value.port();

  return obj;
}

template <>
pi::OctoConfig Marshalling::fromJson(const QJsonObject& obj) {
  return pi::OctoConfig(Marshalling::fromJson<QString>(obj["address"]),
						pi::ApiKey(obj["api_key"].toString()),
						Marshalling::fromJson<QString>(obj["protocol"]),
						obj["port"].toInt());
}
}  // namespace c3picko
