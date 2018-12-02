#include "commands/responses/retrive_response.h"
#include <QJsonArray>

namespace c3picko {
namespace commands {
namespace responses {

RetriveResponse::RetriveResponse(QJsonObject obj) : Response(obj) {
  for (QJsonValue const &file : obj["files"].toArray()) {
    QJsonObject file_obj = file.toObject(); // Why

    files.append(FileInformation(file_obj));
  }

  free = obj["free"].toString();
}
} // namespace responses
} // namespace commands
} // namespace c3picko
