#include "PiCommunicator/responses/upload_response.h"

namespace c3picko {
namespace pi {
namespace responses {
UploadResponse::UploadResponse(const QJsonObject &obj) : Response(obj) {
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
}  // namespace c3picko
