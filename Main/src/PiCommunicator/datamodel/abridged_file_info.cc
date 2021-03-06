/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/datamodel/abridged_file_info.h"

#include <QJsonArray>
#include <QJsonObject>

namespace d3picko {
namespace pi {
namespace data {
AbridgedFileInfo::AbridgedFileInfo(const QJsonObject& obj)
	: responses::Response(obj) {
  name = obj["name"].toString();
  display = obj["display"].toString();
  path = obj["path"].toString();
  origin = data::LocationFromString(obj["origin"].toString());

  if (obj.contains("refs")) ref = obj["refs"].toObject();
}
}  // namespace data
}  // namespace pi
}  // namespace d3picko
