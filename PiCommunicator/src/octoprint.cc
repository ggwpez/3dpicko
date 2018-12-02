#include "octoprint.h"
#include <QJsonArray>
#include <QMetaEnum>
#include <QUrlQuery>

namespace c3picko {
OctoPrint::OctoPrint(QString ip, ApiKey key, QObject *_parent)
    : QObject(_parent), ip_("http://" + ip), apikey_(key),
      network_(new QNetworkAccessManager(this)) {}

void OctoPrint::SendCommand(Command *cmd) {
  QNetworkRequest request(ip_ + "/api/" + cmd->GetApiUrl());
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/json; charset=utf-8");
  request.setRawHeader("X-Api-Key", apikey_.toUtf8());

  QNetworkReply *reply = network_->post(request, cmd->GetPostData());

  QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), cmd,
                   SLOT(OnReplyError(QNetworkReply::NetworkError)));
  QObject::connect(reply, &QNetworkReply::finished,
                   std::bind(&Command::OnReplyFinished, cmd, reply));
  QObject::connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
}
} // namespace c3picko
