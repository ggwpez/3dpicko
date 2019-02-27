/**
  @file
  @author Stefan Frings
*/

#include "httplistener.h"
#include <QCoreApplication>
#include "httpconnectionhandler.h"
#include "httpconnectionhandlerpool.h"

using namespace stefanfrings;

HttpListener::HttpListener(QSettings *settings, QSslConfiguration *ssl,
                           HttpRequestHandler *requestHandler, QObject *parent)
    : QTcpServer(parent), ssl_(ssl) {
  Q_ASSERT(settings != 0);
  Q_ASSERT(requestHandler != 0);
  pool = NULL;
  this->settings = settings;
  this->requestHandler = requestHandler;
  // Reqister type of socketDescriptor for signal/slot handling
  qRegisterMetaType<tSocketDescriptor>("tSocketDescriptor");
  // Start listening
  listen();
}

HttpListener::~HttpListener() {
  close();
  qInfo("HttpListener: destroyed");
}

void HttpListener::listen() {
  if (!pool) {
    pool = new HttpConnectionHandlerPool(settings, ssl_, requestHandler);
  }
  QString host = settings->value("host").toString();
  int port = settings->value("port").toInt();
  QTcpServer::listen(host.isEmpty() ? QHostAddress::Any : QHostAddress(host),
                     port);
  if (!isListening()) {
    qCritical("HttpListener: Cannot bind on port %i: %s", port,
              qPrintable(errorString()));
  } else {
    qDebug("HttpListener up at %s:%i", qPrintable(host), port);
  }
}

void HttpListener::close() {
  QTcpServer::close();
  qInfo("HttpListener: closed");
  if (pool) {
    delete pool;
    pool = NULL;
  }
}

void HttpListener::incomingConnection(tSocketDescriptor socketDescriptor) {
#ifdef SUPERVERBOSE
  qInfo("HttpListener: New connection");
#endif

  HttpConnectionHandler *freeHandler = NULL;
  if (pool) {
    freeHandler = pool->getConnectionHandler();
  }

  // Let the handler process the new connection.
  if (freeHandler) {
    // The descriptor is passed via event queue because the handler lives in
    // another thread
    QMetaObject::invokeMethod(freeHandler, "handleConnection",
                              Qt::QueuedConnection,
                              Q_ARG(tSocketDescriptor, socketDescriptor));
  } else {
    // Reject the connection
    qInfo("HttpListener: Too many incoming connections");
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    socket->write(
        "HTTP/1.1 503 too many connections\r\nConnection: "
        "close\r\n\r\nToo many connections\r\n");
    socket->disconnectFromHost();
  }
}
