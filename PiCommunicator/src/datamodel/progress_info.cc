#include "datamodel/progress_info.h"
#include <QJsonValue>
#include <cstring>

namespace c3picko
{
namespace pi
{
	namespace data
	{
		ProgressInfo::ProgressInfo(QJsonObject obj)
		{
			completion		= obj["completion"].toDouble();
			filepos			= obj["filepos"].toInt();
			print_time		= obj["printTime"].toInt();
			print_time_left = obj["printTimeLeft"].toInt();
		}

		bool ProgressInfo::operator!=(const ProgressInfo& other)
		{
			static_assert(std::is_pod<ProgressInfo>(), "ProgressInfo must be POD");
			// Legit, since it is a POD type
			return std::memcmp(this, std::addressof(other), sizeof(ProgressInfo));
		}
	}
}
}
