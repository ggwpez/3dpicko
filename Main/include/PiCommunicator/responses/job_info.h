#ifndef JOB_INFO_H
#define JOB_INFO_H

#include "PiCommunicator/datamodel/job_general_info.h"
#include "PiCommunicator/datamodel/progress_info.h"
#include "PiCommunicator/response.h"

namespace c3picko {
namespace pi {
namespace responses {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/job.html#get--api-job)
 */
struct JobInfo : public Response {
  JobInfo() = default;
  JobInfo(QJsonObject const&);

  data::JobGeneralInfo job;
  data::ProgressInfo progress;
};
}  // namespace responses
}  // namespace pi
}  // namespace c3picko

#endif  // JOB_INFO_H
