#ifndef JOB_INFO_H
#define JOB_INFO_H

#include "datamodel/job_general_info.h"
#include "datamodel/progress_info.h"
#include "response.h"

namespace c3picko
{
namespace pi
{
	namespace responses
	{
		/**
		 * @brief
		 * [API](http://docs.octoprint.org/en/master/api/job.html#get--api-job)
		 */
		struct JobInfo : public Response
		{
			JobInfo() = default;
			JobInfo(QJsonObject);

			data::JobGeneralInfo job;
			data::ProgressInfo   progress;
		};
	} // namespace responses
} // namespace commands
} // namespace c3picko

#endif // JOB_INFO_H
