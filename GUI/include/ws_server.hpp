#ifndef WS_SERVER_HPP
#define WS_SERVER_HPP

#include "include/api_input.h"
#include <QObject>
#include <QSslConfiguration>
#include <QSslError>

using namespace stefanfrings;

class QWebSocket;
class QWebSocketServer;
namespace c3picko {

class WsServer : public QObject {
  Q_OBJECT
public:
  WsServer(QSettings *settings, QSslConfiguration *ssl,
           QObject *_parent = nullptr);
  ~WsServer();

private slots:
  void NewConnection();
  void NewTextData(QString data);
  void NewBinaryData(QByteArray data);
  void ConnectionClosed();

public slots:

  void StartListen();
  void NewDebugLine(QString line);
  void SslErrors(const QList<QSslError> &errors);

  void ToClient(QObject *client, QString type, QJsonObject data);
  void ToAll(QString type, QJsonObject data);
  void ToAllExClient(QObject *excluded, QString type, QJsonObject data);

public:
  void SendToClient(QWebSocket *client, QString type, QJsonObject packet);
  void ServiceRequestForClient(QJsonObject request, QWebSocket *socket);

signals:
  void OnStarted();
  void OnStopped();
  void OnRequest(QJsonObject request, QString raw_request, QObject *client);

private:
  QWebSocketServer *server_;
  QList<QWebSocket *> clients_;
  QString host_;
};

} // namespace c3picko

#endif // WS_SERVER_HPP
