#ifndef WS_SERVER_HPP
#define WS_SERVER_HPP

#include <QObject>
#include <QtWebSockets>
#include "include/apicontroller.h"

using namespace stefanfrings;

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
