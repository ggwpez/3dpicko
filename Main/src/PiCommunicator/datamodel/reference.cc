/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/datamodel/reference.h"

#include <QJsonObject>

namespace d3picko {
namespace pi {
namespace data {
Reference::Reference(const QJsonObject& obj) {
  resource = obj["resource"].toString();

  if (obj.contains("download")) download = obj["download"].toString();
  if (obj.contains("model")) download = obj["model"].toString();
}
}  // namespace data
}  // namespace pi
}  // namespace d3picko
