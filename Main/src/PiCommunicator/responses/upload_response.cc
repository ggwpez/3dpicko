/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/responses/upload_response.h"

namespace d3picko {
namespace pi {
namespace responses {
UploadResponse::UploadResponse(const QJsonObject& obj) : Response(obj) {
  is_folder = false;

  if (obj.contains("files")) {
	QJsonObject files = obj["files"].toObject();
	local = data::AbridgedFileInfo(files["local"].toObject());

	if (files.contains("sdcard")) {
	  sdcard = files["sdcard"].toObject();
	  origin = data::Location::SD_CARD;
	} else
	  origin = data::Location::LOCAL;
  } else {
	is_folder = true;

	folder = obj["folder"].toObject();
  }

  done = obj["done"].toBool();
}
}  // namespace responses
}  // namespace pi
}  // namespace d3picko
