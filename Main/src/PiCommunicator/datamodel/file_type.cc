/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/datamodel/file_type.h"

#include <QString>

#include "Main/exception.h"

namespace d3picko {
namespace pi {
namespace data {
FileType FileTypeFromString(QString const& str) {
  if (str == "model") return FileType::MODEL;
  if (str == "machinecode") return FileType::MACHINECODE;
  if (str == "folder")
	return FileType::FOLDER;
  else
	throw Exception("Unknown FileType");
}
}  // namespace data
}  // namespace pi
}  // namespace d3picko
