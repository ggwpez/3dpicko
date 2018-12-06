#include "commands/get_all_files.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonParseError>

namespace c3picko {
namespace commands {
GetAllFiles::GetAllFiles(bool recursive)
    : Command("files", QJsonObject({{"recursive", recursive}}), {200}) {}

void GetAllFiles::OnReplyFinished(QNetworkReply *reply) {
  OnReplyFinishedDefault<Response>(reply);
}
} // namespace commands
} // namespace c3picko
