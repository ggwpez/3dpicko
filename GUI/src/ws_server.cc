#include "include/ws_server.hpp"
#include "include/global.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocketServer>
#include <QtWebSockets>

namespace c3picko
{

WsServer::WsServer(QSettings* settings, QSslConfiguration* ssl, APIController* api, QObject* _parent)
	: QObject(_parent), api_(api),
	  server_(new QWebSocketServer(QStringLiteral("Echo Server"), (ssl ? QWebSocketServer::SecureMode : QWebSocketServer::NonSecureMode),
								   this))
{
	if (ssl)
		server_->setSslConfiguration(*ssl);

	QString host = settings->value("host").toString();
	if (server_->listen((host.isEmpty() ? QHostAddress::Any : QHostAddress(host)), 8888))
	{
		qDebug() << "WebSocket server listening on port" << 8888;

		connect(server_, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(SslErrors(const QList<QSslError>&)));
		connect(server_, SIGNAL(newConnection()), this, SLOT(NewConnection()));
		connect(server_, SIGNAL(closed()), this, SIGNAL(OnStopped()));
		emit OnStarted();
	}

	connect(api_, &APIController::OnImageCreated, this, &WsServer::NewFile);
	connect(api_, &APIController::OnJobCreated, this, &WsServer::NewJob);
	connect(api_, &APIController::OnImageDeleted, this, &WsServer::FileDeleted);
	connect(api_, &APIController::OnJobDeleted, this, &WsServer::JobDeleted);
	connect(api_, &APIController::OnImageCreateError, this, &WsServer::FileUploadError);
	connect(api_, &APIController::OnJobCreateError, this, &WsServer::JobCreateError);
	connect(api_, &APIController::OnImageDeleteError, this, &WsServer::FileDeleteError);
	connect(api_, &APIController::OnJobDeleteError, this, &WsServer::JobDeleteError);
	connect(api_, &APIController::OnImageCropped, this, &WsServer::FileCropped);
	connect(api_, &APIController::OnImageCropError, this, &WsServer::FileCropError);

	for (int i = 0; i < APIController::staticMetaObject.methodOffset(); ++i)
	{
		QMetaMethod f = api_->metaObject()->method(i);

		if (f.methodType() == QMetaMethod::Signal)
		{
//			this->metaObject()->me
		}
	}
}

void WsServer::NewConnection()
{
	QWebSocket* pSocket = server_->nextPendingConnection();
	if (! pSocket)
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
		qFatal("%s", "qobject_cast<QWebSocket*>(sender()) was null"); // if this fails, check that this function was never manually called
																	  // but only as slot
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

void WsServer::NewFile(Image img, QObject* socket)
{
	qDebug() << "New image #" << img.path();
	QJsonObject json = api_->createImageList(img);

	for (QWebSocket* client : clients_)
	{
		if (client != socket)
			this->SendToClient("getimagelist", json, client);
	}
}

void WsServer::NewJob(Job job, QObject* socket)
{
	qDebug() << "New job #" << job.id();
	QJsonObject json = api_->createJobList(job);

	for (QWebSocket* client : clients_)
	{
		if (client != socket)
			this->SendToClient("getjoblist", json, client);
	}
}

void WsServer::FileDeleted(Image img, QObject*)
{
	qDebug() << "Deleting image #" << img.id();
	QJsonObject json = api_->createDeleteImage(img);

	for (QWebSocket* client : clients_)
		this->SendToClient("deleteimage", json, client);
}

void WsServer::JobDeleted(Job job, QObject*)
{
	qDebug() << "Deleting job #" << job.id();
	QJsonObject json = api_->createDeleteJob(job);

	for (QWebSocket* client : clients_)
		this->SendToClient("deletejob", json, client);
}

void WsServer::FileUploadError(QString path, QObject* client) { qDebug() << "Error uploading image #" << path; }

void WsServer::JobCreateError(QString path, QObject* client) { qDebug() << "Error creating job #" << path; }

void WsServer::FileDeleteError(QString path, QObject* client) { qDebug() << "Error deleting image #" << path; }

void WsServer::JobDeleteError(QString path, QObject*) { qDebug() << "Error deleting job #" << path; }

void WsServer::FileCropped(Image img, QObject*)
{
	qDebug() << "File cropped " << img.id();

	QJsonObject json = api_->createImageList(img);

	for (QWebSocket* client : clients_)
		this->SendToClient("getimagelist", json, client);
}

void WsServer::FileCropError(QString id, QObject*) { qDebug() << "File crop error " << id; }

void WsServer::NewDebugLine(QString line)
{
	for (auto client : clients_)
		SendToClient("debug", {{"line", line}}, client);
}

void WsServer::SslErrors(const QList<QSslError>& errors)
{
	for (QSslError error : errors)
		qWarning() << "Ssl error:" << error.errorString();
}

void WsServer::SendToClient(QJsonValue type, JsonConvertable& data, QWebSocket* client)
{
	QJsonObject json;
	data.write(json);

	SendToClient(type, json, client);
}

void WsServer::SendToClient(QJsonValue type, QJsonObject data, QWebSocket* client)
{
	QJsonObject json = {{"type", type}, {"data", data}};

	SendToClient(json, client);
}

void WsServer::SendToClient(QJsonObject packet, QWebSocket* client) { client->sendTextMessage(QJsonDocument(packet).toJson()); }

void WsServer::ServiceRequestForClient(QJsonObject request, QWebSocket* socket)
{
	QJsonObject json_data, json;
	api_->service(request, json_data, socket);

	if (!json_data.empty())
		SendToClient(request["request"], json_data, socket);
}

WsServer::~WsServer()
{
	server_->close();
	server_->deleteLater();
	qDeleteAll(clients_);
}
}
