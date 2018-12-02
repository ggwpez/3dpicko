#ifndef FILE_INFORMATION_H_
#define FILE_INFORMATION_H_

#include <QJsonObject>
#include <QString>
#include <QStringList>

namespace c3picko {
namespace commands {
namespace responses {
/**
 * @brief @link
 * http://docs.octoprint.org/en/master/api/datamodel.html#sec-api-datamodel-files-file
 * @endlink
 * TODO there may be additional properties see link above
 */
struct FileInformation {
  FileInformation() = default;
  FileInformation(QJsonObject obj);

  QString name, display, path, type;
  QStringList type_path;
};
} // namespace responses
} // namespace commands
} // namespace c3picko

#endif // FILE_INFORMATION_H_
