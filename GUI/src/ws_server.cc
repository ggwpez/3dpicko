#include "include/ws_server.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocketServer>
#include <QtWebSockets>
#include "include/api_controller.h"
#include "include/global.h"

namespace c3picko {

WsServer::WsServer(QSettings& settings, QSslConfiguration* ssl,
                   QObject* _parent)
    : QObject(_parent),
      server_(new QWebSocketServer(QStringLiteral("Echo Server"),
                                   (ssl ? QWebSocketServer::SecureMode
                                        : QWebSocketServer::NonSecureMode),
                                   this)) {
  if (ssl) {
    Q_ASSERT(ssl);
    server_->setSslConfiguration(*ssl);
  }

  if (!settings.contains("host")) {
    qWarning().nospace() << "WsServer: hostname not set, using default value (="
                         << defaultHost() << ")";
    host_ = defaultHost();
  } else
    host_ = settings.value("host", defaultHost()).toString();

  int port;
  if (!settings.contains("port")) {
    qWarning().nospace() << "WsServer: port not set, using default (="
                         << defaultPort() << ")";
    port = defaultPort();
  } else
    port = settings.value("port").toInt();

  if (port >= (1 << 16) || port < 0) {
    port_ = defaultPort();
    qWarning() << "Port invalid (" << port << "), defaulting to" << port_;
  } else
    port_ = quint16(port);
}

void WsServer::NewConnection() {
  QWebSocket* pSocket = server_->nextPendingConnection();
  if (!pSocket) return;

  connect(pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
          SLOT(clientError(QAbstractSocket::SocketError)));
  connect(pSocket, SIGNAL(textMessageReceived(QString)), this,
          SLOT(NewTextData(QString)));
  connect(pSocket, SIGNAL(binaryMessageReceived(QByteArray)), this,
          SLOT(NewBinaryData(QByteArray)));
  connect(pSocket, SIGNAL(disconnected()), this, SLOT(ConnectionClosed()));

  clients_ << pSocket;
  // qDebug() << "### NewClient" << pSocket;
}

void WsServer::NewTextData(QString data) {
  QWebSocket* client = qobject_cast<QWebSocket*>(sender());

  // qDebug() << "Text data" << data;
  if (client) {
    QJsonObject req = QJsonDocument::fromJson(data.toUtf8()).object();

    emit OnRequest(req, data, client);
  } else
    qFatal("%s",
           "qobject_cast<QWebSocket*>(sender()) was null");  // if this fails,
                                                             // check that this
                                                             // function was
                                                             // never manually
                                                             // called but only
                                                             // as slot
}

void WsServer::NewBinaryData(QByteArray) {
  QWebSocket* client = qobject_cast<QWebSocket*>(sender());

  if (client) {
    qWarning() << "Binary from client ignored" << client;
  }
}

void WsServer::ConnectionClosed() {
  QWebSocket* client = qobject_cast<QWebSocket*>(sender());

  // qDebug() << "Client disconnected" << client;
  if (client) {
    clients_.removeAll(client);
    client->deleteLater();
  }
}

void WsServer::clientError(QAbstractSocket::SocketError ec) {
  qDebug() << "WsServer::clientError =" << ec;
}

bool WsServer::StartListen() {
  if (server_->listen(QHostAddress(host_), port_)) {
    qDebug("WebSocket server at %s:%d", qPrintable(host_), port_);

    connect(server_, &QWebSocketServer::acceptError, this,
            &WsServer::acceptError);
    connect(server_, &QWebSocketServer::peerVerifyError, this,
            &WsServer::peerVerifyError);
    connect(server_, &QWebSocketServer::sslErrors, this, &WsServer::sslErrors);
    connect(server_, &QWebSocketServer::serverError, this,
            &WsServer::serverError);
    connect(server_, SIGNAL(newConnection()), this, SLOT(NewConnection()));
    connect(server_, SIGNAL(closed()), this, SIGNAL(OnStopped()));
    emit OnStarted();
    return true;
  } else {
    qCritical("WebSocket: %s", qPrintable(server_->errorString()));
    return false;
  }
}

void WsServer::NewDebugLine(QString line) {
  for (auto client : clients_) SendToClient(client, "debug", {{"line", line}});
}

void WsServer::acceptError(QAbstractSocket::SocketError ec) {
  qWarning() << "WsServer::acceptError " << ec;
}

void WsServer::peerVerifyError(const QSslError& error) {
  qWarning() << "WsServer::peerVerifyError " << error;
}

void WsServer::serverError(QWebSocketProtocol::CloseCode ec) {
  qWarning() << "WsServer::serverError " << ec;
}

void WsServer::sslErrors(const QList<QSslError>& errors) {
  for (QSslError error : errors)
    qWarning() << "SSL error:" << error.errorString();
}

void WsServer::ToClient(QObject* client, QString type, QJsonObject data) {
  // Is the client still valid?
  if (clients_.contains(reinterpret_cast<QWebSocket*>(client)))
    SendToClient(qobject_cast<QWebSocket*>(client), type, data);
  else
    qWarning() << "Failed to send data to disconnected client";
}

void WsServer::ToAll(QString type, QJsonObject data) {
  for (auto client : clients_) SendToClient(client, type, data);
}

void WsServer::ToAllExClient(QObject* excluded, QString type,
                             QJsonObject data) {
  for (auto client : clients_) {
    if (client != excluded) SendToClient(client, type, data);
  }
}

void WsServer::SendToClient(QWebSocket* client, QString type,
                            QJsonObject packet) {
  QByteArray data = QJsonDocument({{"type", type}, {"data", packet}}).toJson();

  if (!client->isValid())
    qWarning() << "Failed to send data to closed socket";
  else if (client->sendTextMessage(data) != data.size())
    qWarning() << "Failed to send all data to socket";
}

QString WsServer::defaultHost() { return "0.0.0.0"; }

quint16 WsServer::defaultPort() { return 8888; }

WsServer::~WsServer() {
  server_->close();
  server_->deleteLater();
  qDeleteAll(clients_);
}
}  // namespace c3picko
