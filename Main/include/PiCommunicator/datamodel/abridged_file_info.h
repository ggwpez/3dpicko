/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef ABRIDGED_FILE_INFORMATION_H_
#define ABRIDGED_FILE_INFORMATION_H_

#include <QString>

#include "PiCommunicator/datamodel/location.h"
#include "PiCommunicator/datamodel/reference.h"
#include "PiCommunicator/response.h"

class QJsonObject;

namespace d3picko {
namespace pi {
namespace data {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#sec-api-datamodel-files-file)
 *
 * Sadly this is a datamodel and a response, so it needs to be both…
 */
struct AbridgedFileInfo : public responses::Response {
  AbridgedFileInfo() = default;
  AbridgedFileInfo(QJsonObject const& obj);

  QString name, display, path;
  data::Location origin;
  data::Reference ref;
};
}  // namespace data
namespace responses {
typedef data::AbridgedFileInfo AbridgedFileInfo;
}
}  // namespace pi
}  // namespace d3picko

#endif	// ABRIDGED_FILE_INFORMATION_H_
