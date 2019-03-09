#include "include/algorithm_manager.h"
#include "include/algorithms/fluro1.h"
#include "include/algorithms/normal1.h"
#include "include/api_controller.h"
#include "include/database.hpp"
#include "include/requestmapper.h"
#include "include/signal_daemon.h"
#include "include/ws_server.hpp"

#include "httplistener.h"
#include "staticfilecontroller.h"

#include <QCoreApplication>
#include <QFile>
#include <QThreadPool>
#include <QtGlobal>

using namespace stefanfrings;
using namespace c3picko;

static WsServer* ws_ptr = nullptr;
static void msg_handler(QtMsgType type, const QMessageLogContext& context,
                        const QString& msg) {
  QString formated = qFormatLogMessage(type, context, msg);
  QString now = QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate);
  formated = "[ " + now + " ] " + formated;

  if (ws_ptr)
    QMetaObject::invokeMethod(ws_ptr, "NewDebugLine", Q_ARG(QString, formated));

  fprintf(stderr, "%s\n", qPrintable(formated));
}

static int start(int argc, char** argv) {
#ifdef Q_OS_LINUX
  qInstallMessageHandler(msg_handler);
#endif
  QCoreApplication app(argc, argv);
  // FIXME memory managment
  QString ini_file = searchConfigFile(argc, argv);
  QSettings settings(ini_file, QSettings::IniFormat);
  Setup(&app, ini_file, settings);

  QThreadPool* algo_pool = new QThreadPool(&app);
  algo_pool->setMaxThreadCount(qMin(1, QThread::idealThreadCount() / 2));
  AlgorithmManager* detector =
      new AlgorithmManager(algo_pool, {new Normal1(), new Fluro1()}, &app);

  settings.beginGroup("database");
  Database* db = new Database(settings, &app);
  APIController* api = new APIController(detector, db, &app);

  // Static file controller
  settings.beginGroup("files");
  StaticFileController* staticFileController =
      new StaticFileController(settings, DocRoot().toSystemAbsolute(), &app);
  qDebug() << "DocRoot" << DocRoot().toSystemAbsolute();

  // SSL
  settings.beginGroup("ssl");

  QSslConfiguration* ssl = nullptr;
  if (settings.value("enabled", false).toBool()) {
    ssl = LoadSslConfig(settings);
    if (!ssl) {
      qCritical() << "SSL setup failed";
      return 1;
    }
    qDebug() << "SSL setup completed";
  } else
    qDebug() << "SSL disabled";

  // HTTP server
  QSettings* http_settings =
      new QSettings(ini_file, QSettings::IniFormat, &app);
  http_settings->beginGroup("http");
  new HttpListener(http_settings, ssl,
                   new RequestMapper(staticFileController, &app), &app);
  // WS server
  settings.beginGroup("websockets");
  WsServer* ws_server = new WsServer(settings, ssl, &app);

  QObject::connect(ws_server, &WsServer::OnRequest, api,
                   &APIController::request);
  QObject::connect(api, &APIController::toClient, ws_server,
                   &WsServer::ToClient);
  QObject::connect(api, &APIController::toAll, ws_server, &WsServer::ToAll);
  QObject::connect(api, &APIController::toAllExClient, ws_server,
                   &WsServer::ToAllExClient);

  QObject::connect(&app, &QCoreApplication::aboutToQuit, [db] {
    db->saveToFile();                 // save database
    qInstallMessageHandler(nullptr);  // reset message handlers
    ws_ptr =
        nullptr;  // also dont redirect the console output to WsServer anymore
  });

  if (!ws_server->StartListen())
    return 1;
  else
    ws_ptr = ws_server;

  return app.exec();
}

int main(int argc, char** argv) {
  int status;

  // Restarts the program when it exited with exitRestart()
  while (true) {
    qDebug("Starting");
    try {
      status = start(argc, argv);
    } catch (std::exception const& e) {
      qCritical("Exception: %s", e.what());
    } catch (...) {
      qCritical("Exception: %s", "unknown");
    }

    if (status == exitRestart())
      qDebug() << "Awaiting restart...";
    else
      break;
  };

  return status;
}
