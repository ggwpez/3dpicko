#include "commands/get_all_files.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QByteArray>

namespace c3picko {
namespace commands {

	GetAllFiles::GetAllFiles(bool recursive)
		: Command("files", QJsonObject({{"recursive", recursive}}),
				  {200}) {}

	void GetAllFiles::OnReplyFinished(QNetworkReply* reply) {
	QByteArray data = reply->readAll();

	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(data, &error);
	Response* response = new Response(doc.object());

	CheckStatusCode(reply, response);
}
}
}
