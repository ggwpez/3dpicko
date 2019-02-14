#include "include/ws_server.hpp"
#include "include/api_controller.h"
#include "include/global.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocketServer>
#include <QtWebSockets>

namespace c3picko
{

WsServer::WsServer(QSettings* settings, QSslConfiguration* ssl, QObject* _parent)
	: QObject(_parent), server_(new QWebSocketServer(QStringLiteral("Echo Server"),
													 (ssl ? QWebSocketServer::SecureMode : QWebSocketServer::NonSecureMode), this))
{
	if (ssl)
		server_->setSslConfiguration(*ssl);

	host_ = settings->value("host").toString();
}

void WsServer::NewConnection()
{
	QWebSocket* pSocket = server_->nextPendingConnection();
	if (!pSocket)
		return;

	connect(pSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(NewTextData(QString)));
	connect(pSocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(NewBinaryData(QByteArray)));
	connect(pSocket, SIGNAL(disconnected()), this, SLOT(ConnectionClosed()));

	clients_ << pSocket;
	// qDebug() << "### NewClient" << pSocket;
}

void WsServer::NewTextData(QString data)
{
	QWebSocket* client = qobject_cast<QWebSocket*>(sender());

	// qDebug() << "Text data" << data;
	if (client)
	{
		QJsonObject req = QJsonDocument::fromJson(data.toUtf8()).object();
		ServiceRequestForClient(req, client);
	}
	else
		qFatal("%s", "qobject_cast<QWebSocket*>(sender()) was null"); // if this fails, check
																	  // that this function
																	  // was never manually
																	  // called but only as
																	  // slot
}

void WsServer::NewBinaryData(QByteArray data)
{
	QWebSocket* client = qobject_cast<QWebSocket*>(sender());

	if (client)
	{
		qWarning() << "Binary from client ignored " << client;
	}
}

void WsServer::ConnectionClosed()
{
	QWebSocket* client = qobject_cast<QWebSocket*>(sender());

	// qDebug() << "Client disconnected" << client;
	if (client)
	{
		clients_.removeAll(client);
		client->deleteLater();
	}
}

void WsServer::StartListen()
{
	if (server_->listen((host_.isEmpty() ? QHostAddress::Any : QHostAddress(host_)), 8888))
	{
		qDebug() << "WebSocket server listening on port" << 8888;

		connect(server_, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(SslErrors(const QList<QSslError>&)));
		connect(server_, SIGNAL(newConnection()), this, SLOT(NewConnection()));
		connect(server_, SIGNAL(closed()), this, SIGNAL(OnStopped()));
		emit OnStarted();
	}
}

void WsServer::NewDebugLine(QString line)
{
	for (auto client : clients_)
		SendToClient(client, "debug", {{"line", line}});
}

void WsServer::SslErrors(const QList<QSslError>& errors)
{
	for (QSslError error : errors)
		qWarning() << "Ssl error:" << error.errorString();
}

void WsServer::ToClient(QObject* client, QString type, QJsonObject data) { SendToClient(qobject_cast<QWebSocket*>(client), type, data); }

void WsServer::ToAll(QString type, QJsonObject data)
{
	for (auto client : clients_)
		SendToClient(client, type, data);
}

void WsServer::ToAllExClient(QObject* excluded, QString type, QJsonObject data)
{
	for (auto client : clients_)
	{
		if (client != excluded)
			SendToClient(client, type, data);
	}
}

void WsServer::SendToClient(QWebSocket* client, QString type, QJsonObject packet)
{
	client->sendTextMessage(QJsonDocument({{"type", type}, {"data", packet}}).toJson());
}

void WsServer::ServiceRequestForClient(QJsonObject request, QWebSocket* socket)
{
	// QJsonObject json_data, json;
	emit OnRequest(request, socket);

	// if (!json_data.empty())
	// SendToClient(request["request"], json_data, socket);
}

WsServer::~WsServer()
{
	server_->close();
	server_->deleteLater();
	qDeleteAll(clients_);
}
} // namespace c3picko
