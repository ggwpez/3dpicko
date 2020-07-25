#include "PiCommunicator/octoprint.h"

#include <QHostAddress>
#include <QJsonArray>
#include <QMetaEnum>
#include <QUrlQuery>
#include <functional>
#include <utility>

#include "PiCommunicator/command.h"

namespace c3picko {
namespace pi {
OctoPrint::OctoPrint(OctoConfig const& config, QObject* _parent)
	: QObject(_parent),
	  config_(config),
	  network_(new QNetworkAccessManager(this)),
	  resolve_status_(ResolveStatus::UNRESOLVED) {
  /* QUrl url;
   QHostAddress add(config.address());

   if (add.protocol() == QAbstractSocket::IPv4Protocol ||
																																   add.protocol()
   == QAbstractSocket::IPv6Protocol) url.setHost(config.address()); else
   {

   }	 */
}

void OctoPrint::SendCommand(Command* cmd) {
  /*if (resolve_status_ == ResolveStatus::UNRESOLVED)
  {

  }
  else
  {

  }*/

  QNetworkRequest request(config_.protocol() + "://" + config_.address() +
						  "/api/" + cmd->GetApiUrl());
  request.setHeader(QNetworkRequest::ContentTypeHeader, cmd->GetContentType());
  request.setRawHeader("X-Api-Key", config_.key().key().toUtf8());

  QNetworkReply* reply = nullptr;
  switch (cmd->type()) {
	case Command::HTTPType::GET:
	  reply = network_->get(request);
	  break;
	case Command::HTTPType::POST: {
	  if (cmd->IsQuery())
		reply = network_->post(request, cmd->GetPostQuery());
	  else
		reply = network_->post(request, cmd->GetPostData());
	} break;
	case Command::HTTPType::DELETE:
	  reply = network_->deleteResource(request);
	  break;
	default:
	  Q_UNREACHABLE();
  }

  QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), cmd,
				   SLOT(OnReplyError(QNetworkReply::NetworkError)));
  QObject::connect(reply, &QNetworkReply::finished,
				   std::bind(&Command::OnReplyFinished, cmd, reply));
  QObject::connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

void OctoPrint::Resolve() {}
}  // namespace pi
}  // namespace c3picko
