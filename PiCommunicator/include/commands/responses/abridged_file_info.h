#ifndef ABRIDGED_FILE_INFORMATION_H_
#define ABRIDGED_FILE_INFORMATION_H_

#include "commands/responses/datamodel/origin.h"
#include "commands/responses/datamodel/reference.h"
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVector>

namespace c3picko {
namespace commands {
namespace responses {
/**
 * @brief @link
 * http://docs.octoprint.org/en/master/api/datamodel.html#sec-api-datamodel-files-file
 * @endlink
 * TODO there may be additional properties see link above
 */
struct AbridgedFileInfo {
  AbridgedFileInfo() = default;
  AbridgedFileInfo(QJsonObject obj);

  QString name, display, path;
  Origin origin;
  Reference ref;
};
} // namespace responses
} // namespace commands
} // namespace c3picko

#endif // ABRIDGED_FILE_INFORMATION_H_
