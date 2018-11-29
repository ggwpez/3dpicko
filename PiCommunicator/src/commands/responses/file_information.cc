#include "commands/responses/file_information.h"
#include <QJsonArray>

namespace c3picko {
namespace commands {
namespace responses {

FileInformation::FileInformation(QJsonObject obj) {
  name = obj["name"].toString();
  display = obj["display"].toString();
  path = obj["path"].toString();
  type = obj["type"].toString();

  for (QJsonValue const& value : obj["typePath"].toArray())
    type_path.append(value.toString());
}
}
}
}
