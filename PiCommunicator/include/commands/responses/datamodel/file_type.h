#ifndef FILE_TYPE_H
#define FILE_TYPE_H

#include <QString>

namespace c3picko {
namespace commands {
namespace responses {
///
/// [API](http://docs.octoprint.org/en/master/api/datamodel.html#file-information)
///
enum class FileType : char { NONE, MODEL, MACHINECODE, FOLDER };

inline FileType FileTypeFromString(QString str) {
  if (str == "model")
    return FileType::MODEL;
  if (str == "machinecode")
    return FileType::MACHINECODE;
  if (str == "folder")
    return FileType::FOLDER;
  else
    throw std::runtime_error("Unknown FileType");
}
} // namespace responses
} // namespace commands
} // namespace c3picko

#endif // FILE_TYPE_H
