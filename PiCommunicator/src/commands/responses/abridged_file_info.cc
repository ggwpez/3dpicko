#include "commands/responses/abridged_file_info.h"
#include <QJsonArray>

namespace c3picko {
namespace commands {
namespace responses {
AbridgedFileInfo::AbridgedFileInfo(QJsonObject obj) {
  name = obj["name"].toString();
  display = obj["display"].toString();
  path = obj["path"].toString();
  QString orig = obj["origin"].toString();

  if (orig == "local")
    origin = Origin::LOCAL;
  else if (orig == "sdcard")
    origin = Origin::SD_CARD;
  else
    throw std::runtime_error("Value of origin unknown");

  if (obj.contains("refs"))
    ref = obj["refs"].toObject();
}
} // namespace responses
} // namespace commands
} // namespace c3picko
