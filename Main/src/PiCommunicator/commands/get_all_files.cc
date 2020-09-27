/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/commands/get_all_files.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonParseError>

namespace d3picko {
namespace pi {
namespace commands {
GetAllFiles* GetAllFiles::AllFiles() {
  return new GetAllFiles("files?recursive=false", {200},
						 Command::HTTPType::GET);
}

GetAllFiles* GetAllFiles::AllFilesRecursive() {
  return new GetAllFiles("files?recursive=true", {200}, Command::HTTPType::GET);
}

void GetAllFiles::OnReplyFinished(QNetworkReply* reply) {
  CheckStatusCodeAndResponse<Response>(reply);
}
}  // namespace commands
}  // namespace pi
}  // namespace d3picko
