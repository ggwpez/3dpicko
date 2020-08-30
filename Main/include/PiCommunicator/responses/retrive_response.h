/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef RETRIVE_RESPONSE_H_
#define RETRIVE_RESPONSE_H_

#include <QVector>

#include "PiCommunicator/response.h"
#include "PiCommunicator/responses/file_or_folder_info.h"

namespace c3picko {
namespace pi {
namespace responses {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/files.html#sec-api-fileops-datamodel-retrieveresponse)
 */
struct RetriveResponse : public Response {
  RetriveResponse(QJsonObject const&);

  QVector<FileOrFolderInfo> files;
  /**
   * @brief Not in the documentation
   */
  qint64 total;
  qint64 free;
};
}  // namespace responses
}  // namespace pi
}  // namespace c3picko

#endif	// RETRIVE_RESPONSE_H_
