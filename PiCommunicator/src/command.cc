#include "command.h"
#include <QMetaEnum>

namespace c3picko
{
Command::Command(QString api_url, QByteArray data, QSet<int> status_ok, QString content_type)
	: api_url_(api_url), data_(data), status_ok_(status_ok), content_type_(content_type)
{
}

Command::Command(QString api_url, QJsonObject data, QSet<int> status_ok, QString content_type)
	: api_url_(api_url), status_ok_(status_ok), content_type_(content_type)
{
	QJsonDocument doc(data);
	data_ = doc.toJson();
}

Command::~Command()
{
	if (answer_)
		delete answer_;
}

QString Command::GetApiUrl() const { return api_url_; }

QByteArray Command::GetPostData() const { return data_; }

QString Command::GetContentType() const { return content_type_; }

void Command::CheckStatusCode(QNetworkReply* reply, Command::Response* answer)
{
	if (this->answer_)
		emit OnNetworkErr("Cant sent same command twice");
	answer_ = answer;
	// Webservers can return various types as status code
	QVariant status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	if (status.canConvert<int>())
	{
		int code = status.toInt();

		if (status_ok_.contains(code))
			emit OnStatusOk(code, answer);
		else
			emit OnStatusErr(code, answer);
	}
	else
		emit OnStatusErr("The webserver returned a non int HTTP status code " + status.toString(), answer);
}

void Command::OnReplyFinished(QNetworkReply* reply)
{
	CheckStatusCode(reply);

	// The default case is that commands dont have Reply data, so we just
	// ignore the data from reply->readAll()
}

void Command::OnReplyError(QNetworkReply::NetworkError error)
{
	QMetaEnum menum = QMetaEnum::fromType<QNetworkReply::NetworkError>();

	emit OnNetworkErr(menum.valueToKey(error));
}
} // namespace c3picko
