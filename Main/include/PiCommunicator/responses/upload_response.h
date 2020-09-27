/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef UPLOAD_RESPONSE_H_
#define UPLOAD_RESPONSE_H_

#include <QVector>

#include "PiCommunicator/datamodel/abridged_file_info.h"
#include "PiCommunicator/datamodel/location.h"
#include "PiCommunicator/response.h"

namespace d3picko {
namespace pi {
namespace responses {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/files.html#sec-api-fileops-datamodel-retrieveresponse)
 */
struct UploadResponse : public Response {
  UploadResponse(QJsonObject const& obj);

  data::AbridgedFileInfo local, sdcard, folder;
  bool done;

  /// Not in the documentation but needed to remember the type
  data::Location origin;
  /// Is it a folder? Otherwise its a file
  bool is_folder;
};
}  // namespace responses
}  // namespace pi
}  // namespace d3picko

#endif	// RETRIVE_RESPONSE_H_
