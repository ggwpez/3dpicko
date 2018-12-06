#ifndef COMMAND__INC_H_
#define COMMAND_INC_H_

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace c3picko
{
template <typename T> inline void Command::OnReplyFinishedDefault(QNetworkReply* reply)
{
	QByteArray data = reply->readAll();

	QJsonParseError error;
	QJsonDocument   doc		 = QJsonDocument::fromJson(data, &error);
	T*				response = new T(doc.object());

	CheckStatusCode(reply, response);
}
}
#endif // COMMAND_INC_H_
