#include "octoprint.h"
#include <QJsonArray>
#include <QMetaEnum>
#include <QUrlQuery>

namespace c3picko
{
namespace pi
{
	OctoPrint::OctoPrint(QString ip, ApiKey key, QObject* _parent)
		: QObject(_parent), ip_("http://" + ip), apikey_(key), network_(new QNetworkAccessManager(this))
	{
	}

	void OctoPrint::SendCommand(Command* cmd)
	{
		QNetworkRequest request(ip_ + "/api/" + cmd->GetApiUrl());
		request.setHeader(QNetworkRequest::ContentTypeHeader, cmd->GetContentType());
		request.setRawHeader("X-Api-Key", apikey_.toUtf8());

		QNetworkReply* reply = nullptr;
		switch (cmd->type())
		{
			case Command::HTTPType::GET:
				reply = network_->get(request);
				break;
			case Command::HTTPType::POST:
			{
				if (cmd->IsQuery())
					reply = network_->post(request, cmd->GetPostQuery());
				else
					reply = network_->post(request, cmd->GetPostData());
			}
			break;
			case Command::HTTPType::DELETE:
				reply = network_->deleteResource(request);
				break;
			default:
				Q_UNREACHABLE();
		}

		QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), cmd, SLOT(OnReplyError(QNetworkReply::NetworkError)));
		QObject::connect(reply, &QNetworkReply::finished, std::bind(&Command::OnReplyFinished, cmd, reply));
		QObject::connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
	}
}
} // namespace c3picko
