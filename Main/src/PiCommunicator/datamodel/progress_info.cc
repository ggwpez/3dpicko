/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/datamodel/progress_info.h"

#include <QJsonObject>
#include <QJsonValue>
#include <cstring>

namespace c3picko {
namespace pi {
namespace data {
ProgressInfo::ProgressInfo(const QJsonObject& obj) {
  completion = obj["completion"].toDouble();
  filepos = obj["filepos"].toInt();
  print_time = obj["printTime"].toInt();
  print_time_left = obj["printTimeLeft"].toInt();
}

bool ProgressInfo::operator!=(const ProgressInfo& other) {
  static_assert(std::is_pod<ProgressInfo>(), "ProgressInfo must be POD");
  // Legit, since it is a POD type
  return std::memcmp(this, std::addressof(other), sizeof(ProgressInfo));
}
}  // namespace data
}  // namespace pi
}  // namespace c3picko
