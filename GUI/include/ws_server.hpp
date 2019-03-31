#ifndef WS_SERVER_HPP
#define WS_SERVER_HPP

#include <qwebsocketprotocol.h>
#include <QObject>
#include <QSslConfiguration>
#include <QSslError>
#include "include/api_input.h"

using namespace stefanfrings;

class QWebSocket;
class QWebSocketServer;
namespace c3picko {

/**
 * @brief Websocket Server. Handles websocket connections and API requests for
 * all clients. Configured via config.ini [websockets]
 */
class WsServer : public QObject {
  Q_OBJECT
 public:
  WsServer(QSettings& settings, QSslConfiguration* ssl,
           QObject* _parent = nullptr);
  ~WsServer();

 public slots:
  /**
   * @brief Send a line of debug output to the client.
   */
  void NewDebugLine(QString line);

  // From API
  /**
   * @brief Sends a message to the client.
   * @param client Target
   * @param type Type of the message.
   * @param data Content of the message.
   */
  void ToClient(QObject* client, QString type, QJsonObject data);
  /**
   * @brief Sends a message to all clients.
   * @param client Sender.
   * @param type Type of the message.
   * @param data Content of the message.
   */
  void ToAll(QString type, QJsonObject data);
  /**
   * @brief Sends a message to all clients except to excluded.
   * @param excluded Client excluded from the send command.
   * @param type Type of the message.
   * @param data Content of the message.
   */
  void ToAllExClient(QObject* excluded, QString type, QJsonObject data);

 private slots:
  // From QWebSocketServer
  void NewConnection();
  void acceptError(QAbstractSocket::SocketError);
  void peerVerifyError(QSslError const&);
  void serverError(QWebSocketProtocol::CloseCode);
  void sslErrors(const QList<QSslError>& errors);

  // From QWebSocket
  void NewTextData(QString data);
  void NewBinaryData(QByteArray);
  void ConnectionClosed();
  void clientError(QAbstractSocket::SocketError ec);

 public:
  /**
   * @brief Starts listening for new WebSocket connections.
   * @return Success
   */
  bool StartListen();

 private:
  void SendToClient(QWebSocket* client, QString type, QJsonObject packet);
  void ServiceRequestForClient(QJsonObject request, QWebSocket* socket);

  static QString defaultHost();
  static quint16 defaultPort();

 signals:
  /**
   * @brief Emitted when the server started listening.
   */
  void OnStarted();
  /**
   * @brief Emitted when the server stopped listening.
   */
  void OnStopped();
  /**
   * @brief Emitted when a request was received from a client.
   *
   * This needs to be connected to the request processing logic, for example the
   * ApiController::request.
   * @param request Request data.
   * @param raw_request Raw request data.
   * @param client Client that the message was received from.
   */
  void OnRequest(QJsonObject request, QString raw_request, QObject* client);

 private:
  QWebSocketServer* server_;
  QList<QWebSocket*> clients_;
  QString host_;
  quint16 port_;
};

}  // namespace c3picko

#endif  // WS_SERVER_HPP
