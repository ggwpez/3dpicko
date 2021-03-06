/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/datamodel/job_general_info.h"

#include <QJsonObject>
#include <QJsonValue>

namespace d3picko {
namespace pi {
namespace data {
JobGeneralInfo::JobGeneralInfo(QJsonObject const& obj) {
  file = AbridgedFileInfo(obj["file"].toObject());

  if (obj.contains("estimatedPrintTime"))
	estimated_print_time = obj["estimatedPrintTime"].toInt();
  if (obj.contains("lastPrintTime"))
	last_print_time = obj["lastPrintTime"].toInt();
  if (obj.contains("filament")) {
	QJsonObject fil = obj["filament"].toObject();

	filament.length = fil["length"].toInt();
	filament.volume = fil["volume"].toDouble();
  }
}
}  // namespace data
}  // namespace pi
}  // namespace d3picko
