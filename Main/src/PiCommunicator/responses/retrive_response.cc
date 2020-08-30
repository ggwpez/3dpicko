/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/responses/retrive_response.h"

#include <QJsonArray>
#include <QJsonValue>
#include <QVariant>

namespace c3picko {
namespace pi {
namespace responses {
RetriveResponse::RetriveResponse(const QJsonObject& obj) : Response(obj) {
  for (QJsonValue const& file : obj["files"].toArray()) {
	QJsonObject file_obj = file.toObject();	 // Why

	files.append(FileOrFolderInfo(file_obj));
  }

  if (obj.contains("total"))
	total =
		obj["total"].toVariant().toLongLong();	// this is why i have Qt Json

  free = obj["free"].toVariant().toLongLong();
}
}  // namespace responses
}  // namespace pi
}  // namespace c3picko
