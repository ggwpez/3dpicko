/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef FILE_TYPE_H
#define FILE_TYPE_H

class QString;

namespace d3picko {
namespace pi {
namespace data {
///
/// [API](http://docs.octoprint.org/en/master/api/datamodel.html#file-information)
///
enum class FileType : char { NONE, MODEL, MACHINECODE, FOLDER };

FileType FileTypeFromString(QString const& str);
}  // namespace data
}  // namespace pi
}  // namespace d3picko

#endif	// FILE_TYPE_H
