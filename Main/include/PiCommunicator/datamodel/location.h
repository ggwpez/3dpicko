/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef ORIGIN_H
#define ORIGIN_H

class QString;

namespace d3picko {
namespace pi {
namespace data {
///
/// [API](http://docs.octoprint.org/en/master/api/datamodel.html#files)
///
enum class Location : char { LOCAL, SD_CARD };

Location LocationFromString(QString const& str);
QString ToString(Location val);
}  // namespace data
}  // namespace pi
}  // namespace d3picko

#endif	// ORIGIN_H
