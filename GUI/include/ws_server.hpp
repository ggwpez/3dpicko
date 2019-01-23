#ifndef WS_SERVER_HPP
#define WS_SERVER_HPP

#include <QObject>
#include "include/apicontroller.h"

using namespace stefanfrings;

class QWebSocket;
class QWebSocketServer;
namespace c3picko {

class WsServer : public QObject
{
	Q_OBJECT
public:
	WsServer(APIController* api, QObject* _parent = nullptr);
	~WsServer();

private slots:
	void NewConnection();
	void NewTextData(QString data);
	void NewBinaryData(QByteArray data);
	void ConnectionClosed();

public slots:
	void NewFile(Image img, QObject* socket);
	void NewJob(Job job, QObject* socket);
	void FileDeleted(Image img, QObject*);
	void JobDeleted(Job job, QObject*);
	void FileUploadError(QString path, QObject* client);
	void JobCreateError(QString path, QObject* client);
	void FileDeleteError(QString path, QObject* client);
	void JobDeleteError(QString path, QObject* client);
	void NewDebugLine(QString line);

public:
	void SendToClient(QJsonValue type, JsonConvertable& data, QWebSocket* client);
	void SendToClient(QJsonValue type, QJsonObject data, QWebSocket* client);
	void SendToClient(QJsonObject packet, QWebSocket* client);
	void ServiceRequestForClient(QJsonObject request, QWebSocket* socket);

signals:
	void OnStarted();
	void OnStopped();

private:
	QWebSocketServer* server_;
	QList<QWebSocket*> clients_;
	APIController* api_;
};

}

#endif // WS_SERVER_HPP
