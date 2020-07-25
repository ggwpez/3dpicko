#include "PiCommunicator/commands/delete_file.h"

namespace c3picko {
namespace pi {
namespace commands {
DeleteFile* DeleteFile::Delete(data::Location location, QString path) {
  return new DeleteFile("files/" + data::ToString(location) + '/' + path, {204},
						HTTPType::DELETE);
}
}  // namespace commands
}  // namespace pi
}  // namespace c3picko
