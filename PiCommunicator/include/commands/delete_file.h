#ifndef DELETE_FILE_H
#define DELETE_FILE_H

#include "command.h"
#include "datamodel/location.h"

namespace c3picko {
namespace pi {
namespace commands {
class DeleteFile : public Command {
  Q_OBJECT

  DeleteFile() = delete;
  using Command::Command;

 public:
  /**
   * @brief Delete a file at @p path on @p location
   *
   * TODO Octoprint docu does not clarify wheter you can also delete folders or
   * not [API](http://docs.octoprint.org/en/master/api/files.html#delete-file)
   * @param location SdCard or Local
   * @param path Path to the file to delete
   */
  static DeleteFile *Delete(data::Location location, QString path);
};
}  // namespace commands
}  // namespace pi
}  // namespace c3picko

#endif  // DELETE_FILE_H
