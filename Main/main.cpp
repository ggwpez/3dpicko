#include "include/algorithm_manager.h"
#include "include/algorithms/fluro1.h"
#include "include/algorithms/normal1.h"
#include "include/algorithms/plate1.h"
#include "include/api_controller.h"
#include "include/database.hpp"
#include "include/requestmapper.h"
#include "include/signal_daemon.h"
#include "include/updater.h"
#include "include/ws_server.hpp"

#include "httplistener.h"
#include "staticfilecontroller.h"

#include <QCoreApplication>
#include <QFile>
#include <QThreadPool>
#include <QtGlobal>

using namespace stefanfrings;
using namespace c3picko;

static int start(int argc, char** argv) {
#ifdef Q_OS_LINUX
  startLog();
#endif
  QCoreApplication app(argc, argv);
  QString ini_file(Setup(&app));
  QSettings settings(ini_file, QSettings::IniFormat);

  QThreadPool* algo_pool = new QThreadPool(&app);
  algo_pool->setMaxThreadCount(qMin(1, QThread::idealThreadCount() / 2));
  AlgorithmManager* colony_detector =
      new AlgorithmManager(algo_pool, {new Normal1(), new Fluro1()}, &app);
  AlgorithmManager* plate_detector =
      new AlgorithmManager(algo_pool, {new Plate1()}, &app);

  settings.beginGroup("database");
  Database* db = new Database(settings, &app);
  APIController* api =
      new APIController(colony_detector, plate_detector, db, &app);

  // Static file controller
  settings.beginGroup("files");
  StaticFileController* staticFileController =
      new StaticFileController(settings, DocRoot().toSystemAbsolute(), &app);
  qInfo() << "DocRoot is" << DocRoot().toSystemAbsolute();

  // SSL
  settings.beginGroup("ssl");
  QSslConfiguration* ssl = LoadSslConfig(settings);

  // HTTP server
  QSettings http_settings(ini_file, QSettings::IniFormat);
  http_settings.beginGroup("http");
  new HttpListener(&http_settings, ssl,
                   new RequestMapper(staticFileController, &app), &app);

  // WS server
  QSettings ws_settings(ini_file, QSettings::IniFormat);
  ws_settings.beginGroup("websockets");
  WsServer* ws_server = new WsServer(ws_settings, ssl, &app);

  QObject::connect(ws_server, &WsServer::OnRequest, api,
                   &APIController::request);
  QObject::connect(api, &APIController::toClient, ws_server,
                   &WsServer::ToClient);
  QObject::connect(api, &APIController::toAll, ws_server, &WsServer::ToAll);
  QObject::connect(api, &APIController::toAllExClient, ws_server,
                   &WsServer::ToAllExClient);

  QObject::connect(&app, &QCoreApplication::aboutToQuit, [db] {
    db->saveToFile();  // save database
    stopLog();         // reset message handlers
  });

  if (!ws_server->StartListen()) return 1;
  QSettings usettings(ini_file, QSettings::IniFormat);
  usettings.beginGroup("updater");
  Updater updater(usettings, *db);

  // copy logging output to ws_server
  setMessageHandler([&ws_server](QString msg) {
    if (ws_server)
      QMetaObject::invokeMethod(ws_server, "NewDebugLine",
                                Q_ARG(QString, msg));  // send to other thread
  });

  return app.exec();
}

int main(int argc, char** argv) {
  int status;

  // Restarts the program when it exited with exitCodeRestart()
  while (true) {
    qInfo("Starting");
    try {
      status = start(argc, argv);
    } catch (std::exception const& e) {
      qCritical("Exception: %s", e.what());
    } catch (...) {
      qCritical("Exception: %s", "unknown");
    }

    if (status == exitCodeRestart())
      qInfo() << "Awaiting restart...";
    else
      break;
  };

  return status;
}
