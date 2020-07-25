#ifndef FILE_TYPE_H
#define FILE_TYPE_H

class QString;

namespace c3picko {
namespace pi {
namespace data {
///
/// [API](http://docs.octoprint.org/en/master/api/datamodel.html#file-information)
///
enum class FileType : char { NONE, MODEL, MACHINECODE, FOLDER };

FileType FileTypeFromString(QString const& str);
}  // namespace data
}  // namespace pi
}  // namespace c3picko

#endif	// FILE_TYPE_H
