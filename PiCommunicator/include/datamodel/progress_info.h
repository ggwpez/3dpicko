#ifndef PROGRESS_INFO_H
#define PROGRESS_INFO_H

#include <QtGlobal>
class QJsonObject;

namespace c3picko {
namespace pi {
namespace data {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#sec-api-datamodel-jobs-progress)
 */
struct ProgressInfo {
  ProgressInfo() = default;
  ProgressInfo(QJsonObject const &);
  bool operator!=(ProgressInfo const &other);

  qreal completion;
  qint32 filepos;
  qint32 print_time;
  qint32 print_time_left;
};
}  // namespace data
}  // namespace pi
}  // namespace c3picko

#endif  // PROGRESS_INFO_H
