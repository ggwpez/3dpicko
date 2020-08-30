/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/apikey.h"

#include "Main/exception.h"

namespace c3picko {
template <>
QJsonObject Marshalling::toJson(const pi::ApiKey& value) {
  QJsonObject obj;

  obj["data"] = value.key();

  return obj;
}

template <>
pi::ApiKey Marshalling::fromJson(const QJsonObject& obj) {
  return pi::ApiKey(Marshalling::fromJson<QString>(obj["data"]));
}
namespace pi {
pi::ApiKey::ApiKey(const QString& key) : key_(key) {
// TODO is this official? The docu does not clarify in what format the key is.
// http://docs.octoprint.org/en/master/api/general.html
#ifndef C3PICKO_API_KEY_CHECK_OFF
  if (key.size() != 32)
	throw Exception(
		"Octoprint API key should have length 32 (disable with "
		"-D3PICKO_API_KEY_CHECK_OFF)");

  QRegExp is_hex("^[0-9A-Fa-f]*$", Qt::CaseInsensitive);
  if (!is_hex.exactMatch(key))
	throw Exception(
		"Octoprint API key should only contain hexadecimal digits (disable "
		"with -D3PICKO_API_KEY_CHECK_OFF)");
#endif
}

QString ApiKey::key() const { return key_; }
}  // namespace pi
}  // namespace c3picko
