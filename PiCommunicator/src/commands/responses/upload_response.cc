#include "include/commands/responses/upload_response.h"

namespace c3picko
{
namespace commands
{
	namespace responses
	{
		UploadResponse::UploadResponse(QJsonObject obj)
		{
			is_folder = false;

			if (obj.contains("files"))
			{
				QJsonObject files = obj["files"].toObject();
				local			  = AbridgedFileInfo(files["local"].toObject());

				if (files.contains("sdcard"))
				{
					sdcard = files["sdcard"].toObject();
					origin = Origin::SD_CARD;
				}
				else
					origin = Origin::LOCAL;
			}
			else
			{
				is_folder = true;

				folder = obj["folder"].toObject();
			}

			done = obj["done"].toBool();
		}
	}
}
}
