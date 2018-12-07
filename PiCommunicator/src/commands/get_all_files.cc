#include "commands/get_all_files.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonParseError>

namespace c3picko {
namespace commands {
GetAllFiles::GetAllFiles(bool recursive)
	: Command("files" +QString(recursive ? "?recursive=true" : ""), QJsonObject(), {200}, Command::Type::GET) {}

void GetAllFiles::OnReplyFinished(QNetworkReply *reply) {
  OnReplyFinishedDefault<Response>(reply);
}
} // namespace commands
} // namespace c3picko
