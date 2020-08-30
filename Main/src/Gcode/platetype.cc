/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 flopicko flobotic@protonmail.com
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Gcode/platetype.h"

namespace d3picko {
QString toString(PlateType type) {
  switch (type) {
	case PlateType::kRECT:
	  return QStringLiteral("kRECT");
	case PlateType::kROUND:
	  return QStringLiteral("kROUND");
	default:
	  Q_UNREACHABLE();
  }
}

PlateType plateTypeFromString(const QString& str) {
  if (str == "kRECT") return PlateType::kRECT;
  if (str == "KROUND") return PlateType::kROUND;

  Q_UNREACHABLE();
}

template <>
QJsonObject Marshalling::toJson(const PlateType& value) {
  return {{"enum", toString(value)}};
}

template <>
PlateType Marshalling::fromJson(const QJsonObject& obj) {
  return plateTypeFromString(obj["enum"].toString());
}
}  // namespace d3picko
