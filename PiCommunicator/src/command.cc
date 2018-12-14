#include "command.h"
#include <QMetaEnum>
#include <QVariant>

namespace c3picko
{
namespace pi
{
	Command::Command(QString api_url, QSet<int> status_ok, Command::HTTPType type)
		: api_url_(api_url), data_(QByteArray()), status_ok_(status_ok), type_(type), content_type_("application/json; charset=utf-8")
	{
		SetupSlots();
	}

	Command::Command(QString api_url, QByteArray data, QSet<int> status_ok, HTTPType type, QString content_type)
		: api_url_(api_url), data_(data), status_ok_(status_ok), type_(type), content_type_(content_type)
	{
		SetupSlots();
	}

	Command::Command(QString api_url, QHttpMultiPart* query, QSet<int> status_ok, HTTPType type, QString content_type)
		: api_url_(api_url), query_(query), status_ok_(status_ok), type_(type), content_type_(content_type)
	{
		SetupSlots();
	}

	Command::~Command()
	{
		if (query_)
			delete query_;
	}

	Command::Command(QString api_url, QJsonObject data, QSet<int> status_ok, HTTPType type, QString content_type)
		: api_url_(api_url), status_ok_(status_ok), type_(type), content_type_(content_type)
	{
		SetupSlots();
		if (data.isEmpty())
			return;

		QJsonDocument doc(data);
		data_ = doc.toJson();
	}

	void Command::SetupSlots()
	{
		connect(this, SIGNAL(OnStatusOk(int, Response*)), this, SIGNAL(OnFinished()));
		connect(this, SIGNAL(OnStatusErr(QVariant, Response*)), this, SIGNAL(OnFinished()));
		connect(this, SIGNAL(OnNetworkErr(QString)), this, SIGNAL(OnFinished()));
	}

	QString Command::GetApiUrl() const { return api_url_; }

	QByteArray Command::GetPostData() const { return data_; }

	QHttpMultiPart* Command::GetPostQuery() const { return query_; }

	QString Command::GetContentType() const { return content_type_; }

	bool Command::IsQuery() const { return query_; }

	void Command::CheckStatusCode(QNetworkReply* reply, Command::Response* answer)
	{
		// Hhe Webserver can return various types as status code
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

	Command::HTTPType Command::type() const { return type_; }

	void Command::OnReplyFinished(QNetworkReply* reply)
	{
		qDebug() << "base";
		CheckStatusCode(reply);

		// The default case is that commands dont have Reply data, so we just
		// ignore the data from reply->readAll()
	}

	void Command::OnReplyError(QNetworkReply::NetworkError error)
	{
		QMetaEnum menum = QMetaEnum::fromType<QNetworkReply::NetworkError>();

		emit OnNetworkErr(menum.valueToKey(error));
	}
}
} // namespace c3picko
