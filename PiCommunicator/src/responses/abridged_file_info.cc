#include "responses/abridged_file_info.h"
#include <QJsonArray>

namespace c3picko
{
namespace pi
{
	namespace responses
	{
		AbridgedFileInfo::AbridgedFileInfo(QJsonObject obj)
		{
			name	= obj["name"].toString();
			display = obj["display"].toString();
			path	= obj["path"].toString();
			origin  = data::LocationFromString(obj["origin"].toString());

			if (obj.contains("refs"))
				ref = obj["refs"].toObject();
		}
	} // namespace responses
} // namespace commands
} // namespace c3picko
