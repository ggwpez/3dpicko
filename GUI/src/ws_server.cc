#include "include/ws_server.hpp"
#include "include/global.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

namespace c3picko {

WsServer::WsServer(APIController* api, QObject* _parent)
	: QObject(_parent), api_(api),
	  server_(new QWebSocketServer(QStringLiteral("Echo Server"),
								   QWebSocketServer::NonSecureMode, this))
{
    if (server_->listen(QHostAddress::LocalHost, 8888))
	{
		qDebug() << "Echoserver listening on port" << 8888;

		connect(server_, SIGNAL(newConnection()), this, SLOT(NewConnection()));
		connect(server_, SIGNAL(closed()), this, SIGNAL(OnStopped()));
		emit OnStarted();
	}

	/*connect(api_, &APIController::OnNewFile, [this](QString fname)
	{
		for (QWebSocket* client : clients_)
			this->ServiceRequestForClient("/getimagelist", client);
	});*/
}

void WsServer::NewConnection()
{
	QWebSocket *pSocket = server_->nextPendingConnection();

	connect(pSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(NewTextData(QString)));
	connect(pSocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(NewBinaryData(QByteArray)));
	connect(pSocket, SIGNAL(disconnected()), this, SLOT(ConnectionClosed()));

	clients_ << pSocket;
	qDebug() << "NewClient" << pSocket;
}

void WsServer::NewTextData(QString data)
{
	QWebSocket* client = qobject_cast<QWebSocket*>(sender());

	qDebug() << "Text data" << data;
	if (client)
	{
		qDebug() << "From client" << client;

		QJsonObject req = QJsonDocument::fromJson(data.toUtf8()).object();
		ServiceRequestForClient(req, client);
	}
}

void WsServer::NewBinaryData(QByteArray data)
{
	QWebSocket* client = qobject_cast<QWebSocket*>(sender());

	qDebug() << "Binary data" << data;
	if (client)
	{
		qDebug() << "From client" << client;
	}
}

void WsServer::ConnectionClosed()
{
	QWebSocket* client = qobject_cast<QWebSocket*>(sender());

	qDebug() << "Client disconnected" << client;
	if (client)
	{
		clients_.removeAll(client);
		client->deleteLater();
	}
}

void WsServer::ServiceRequestForClient(QJsonObject request, QWebSocket* socket)
{
	QJsonObject json_data, json;
	api_->service(request, json_data);
	json = QJsonObject{
		{"type", request["request"]},
		{"data", json_data}
	};

	socket->sendTextMessage(QJsonDocument(json).toJson());
}

WsServer::~WsServer()
{
	server_->close();
	server_->deleteLater();
	qDeleteAll(clients_);
}

}
