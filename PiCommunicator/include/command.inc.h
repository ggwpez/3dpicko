#ifndef COMMAND__INC_H_
#define COMMAND_INC_H_

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace c3picko
{
namespace pi
{
	template <typename T> inline void Command::CheckStatusCodeAndResponse(QNetworkReply* reply)
	{
		QByteArray data = reply->readAll();

		if (data.size() == 0)
		{
			emit OnNetworkErr("Empty response body");
			return;
		}

		QJsonParseError error;
		QJsonDocument   doc		 = QJsonDocument::fromJson(data, &error); // FIXME check error

		if (error.error != QJsonParseError::NoError)
		{
			emit OnNetworkErr("Json parsing failed: " +error.errorString());
			return;
		}

		T*				response = new T(doc.object());

		CheckStatusCode(reply, response);
	}
} // namespace pi
} // namespace c3picko
#endif // COMMAND_INC_H_
