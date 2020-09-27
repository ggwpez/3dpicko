/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/response.h"

#include <QJsonDocument>

namespace d3picko {
namespace pi {
namespace responses {
Response::Response(const QJsonObject& obj) : raw(obj) {}

QString Response::ToString() {
  QJsonDocument doc(raw);
  return doc.toJson();
}
}  // namespace responses
}  // namespace pi
}  // namespace d3picko
