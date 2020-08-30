/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace c3picko {
namespace pi {
template <typename T>
inline void Command::CheckStatusCodeAndResponse(QNetworkReply* reply) {
  QByteArray data = reply->readAll();

  if (data.size() == 0) {
	emit OnNetworkErr("Empty response body");
	return;
  }

  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(data, &error);

  if (error.error != QJsonParseError::NoError) {
	emit OnNetworkErr("Json parsing failed: " + error.errorString());
	return;
  }

  T* response = new T(doc.object());

  CheckStatusCode(reply, response);
}
}  // namespace pi
}  // namespace c3picko
