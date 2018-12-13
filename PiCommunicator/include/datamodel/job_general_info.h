#ifndef JOBGENERALINFO_H
#define JOBGENERALINFO_H

#include "datamodel/abridged_file_info.h"
#include <QJsonObject>

namespace c3picko
{
namespace pi
{
	namespace data
	{
		/**
		 * @brief [API](http://docs.octoprint.org/en/master/api/datamodel.html#job-information)
		 */
		struct JobGeneralInfo
		{
			JobGeneralInfo() = default;
			JobGeneralInfo(QJsonObject);

			AbridgedFileInfo file;
			qint32			 estimated_print_time;
			qint32			 last_print_time;

			struct
			{
				qint32 length;
				qreal  volume;
			} filament;
		};
	} // namespace data
} // namespace commands
} // namespace c3picko
#endif // JOBGENERALINFO_H
