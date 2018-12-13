#include "responses/job_info.h"
#include <QJsonValue>

namespace c3picko
{
namespace pi
{
	namespace responses
	{
		JobInfo::JobInfo(QJsonValue obj)
		{
			job		 = data::JobGeneralInfo(obj["job"].toObject());
			progress = data::ProgressInfo(obj["progress"].toObject());
		}
	}
}
}
