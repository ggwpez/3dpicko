/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef REFERENCE_H
#define REFERENCE_H

#include <QUrl>
class QJsonObject;

namespace d3picko {
namespace pi {
namespace data {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#references)
 */
struct Reference {
  Reference() = default;
  Reference(QJsonObject const& obj);

  QUrl resource, download, model;
};
}  // namespace data
}  // namespace pi
}  // namespace d3picko

#endif	// REFERENCE_H
