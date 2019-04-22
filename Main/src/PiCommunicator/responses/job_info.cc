#include "PiCommunicator/responses/job_info.h"
#include <QJsonValue>

namespace c3picko {
namespace pi {
namespace responses {
JobInfo::JobInfo(const QJsonObject &obj) : Response(obj) {
  job = data::JobGeneralInfo(obj["job"].toObject());
  progress = data::ProgressInfo(obj["progress"].toObject());
}
}  // namespace responses
}  // namespace pi
}  // namespace c3picko
