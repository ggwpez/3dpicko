/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/commands/file_operation.h"

namespace d3picko {
namespace pi {
namespace commands {
FileOperation* FileOperation::select(data::Location location, QString path,
									 bool print) {
  // TODO API says 200 on one site and 204 on the other
  return new FileOperation(
	  "files/" + data::ToString(location) + '/' + path,
	  {{"command", "select"}, {"print", print ? "true" : "false"}}, {200, 204},
	  HTTPType::POST);
}

FileOperation* FileOperation::copy(data::Location location, QString origin,
								   QString destination) {
  return new FileOperation("files/" + data::ToString(location) + '/' + origin,
						   {{"command", "copy"}, {"destination", destination}},
						   {201}, HTTPType::POST);
}

FileOperation* FileOperation::move(data::Location location, QString origin,
								   QString destination) {
  return new FileOperation("files/" + data::ToString(location) + '/' + origin,
						   {{"command", "move"}, {"destination", destination}},
						   {201}, HTTPType::POST);
}

void FileOperation::OnReplyFinished(QNetworkReply* reply) {
  if (!status_ok_.contains(200))
	CheckStatusCodeAndResponse<Response>(reply);
  else
	CheckStatusCode(reply);
}
}  // namespace commands
}  // namespace pi
}  // namespace d3picko
