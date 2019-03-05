#include "datamodel/file_type.h"
#include <QString>
#include "include/exception.h"

namespace c3picko {
namespace pi {
namespace data {
FileType FileTypeFromString(QString const& str) {
  if (str == "model") return FileType::MODEL;
  if (str == "machinecode") return FileType::MACHINECODE;
  if (str == "folder")
    return FileType::FOLDER;
  else
    throw Exception("Unknown FileType");
}
}  // namespace data
}  // namespace pi
}  // namespace c3picko
