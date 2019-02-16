#ifndef WS_SERVER_HPP
#define WS_SERVER_HPP

#include "include/api_input.h"
#include <QObject>
#include <QSslConfiguration>
#include <QSslError>
#include <QWebSocketProtocol>

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
  // From QWebSocketServer
  void NewConnection();
  void acceptError(QAbstractSocket::SocketError);
  void peerVerifyError(QSslError const &);
  void serverError(QWebSocketProtocol::CloseCode);
  void sslErrors(const QList<QSslError> &errors);

  // From QWebSocket
  void NewTextData(QString data);
  void NewBinaryData(QByteArray data);
  void ConnectionClosed();
  void clientError(QAbstractSocket::SocketError ec);

public slots:

  bool StartListen();
  void NewDebugLine(QString line);

  void ToClient(QObject *client, QString type, QJsonObject data);
  void ToAll(QString type, QJsonObject data);
  void ToAllExClient(QObject *excluded, QString type, QJsonObject data);

public:
  void SendToClient(QWebSocket *client, QString type, QJsonObject packet);
  void ServiceRequestForClient(QJsonObject request, QWebSocket *socket);

  static QString defaultHost();
  static quint16 defaultPort();

signals:
  void OnStarted();
  void OnStopped();
  void OnRequest(QJsonObject request, QString raw_request, QObject *client);

private:
  QSettings *settings_;
  QWebSocketServer *server_;
  QList<QWebSocket *> clients_;
  QString host_;
  quint16 port_;
};

} // namespace c3picko

#endif // WS_SERVER_HPP
