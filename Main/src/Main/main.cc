/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <QCoreApplication>
#include <QFile>
#include <QThreadPool>
#include <QtGlobal>

#include "GUI/api_controller.h"
#include "GUI/autosaver.h"
#include "GUI/database.h"
#include "GUI/requestmapper.h"
#include "GUI/ws_server.h"
#include "ImageRecognition/algorithm_manager.h"
#include "ImageRecognition/algorithms/colonies1.h"
#include "ImageRecognition/algorithms/fluro1.h"
#include "ImageRecognition/algorithms/plate_rect.h"
#include "ImageRecognition/algorithms/plate_round.h"
#include "Main/dtor_callback.h"
#include "Main/logger.h"
#include "Main/signal_daemon.h"
#include "Main/updater.h"
#include "httplistener.h"
#include "staticfilecontroller.h"

using namespace stefanfrings;
using namespace d3picko;
using namespace d3picko::algorithms;

static int start(int argc, char** argv) {
#ifdef Q_OS_LINUX
  startLog();
#endif
  QCoreApplication app(argc, argv);
  QString ini_file(setupGlobal(&app));
  QSettings settings(ini_file, QSettings::IniFormat);

  // Algorithms
  QThreadPool* algo_pool = new QThreadPool(&app);
  algo_pool->setMaxThreadCount(qMax(1, QThread::idealThreadCount()));
  AlgorithmManager* colony_detector =
	  new AlgorithmManager(algo_pool, {new Colonies1(), new Fluro1()}, &app);
  AlgorithmManager* plate_detector = new AlgorithmManager(
	  algo_pool, {new PlateRect(), new PlateRound()}, &app);

  // Database
  settings.beginGroup("database");
  Database* db = new Database(settings, &app);
  settings.endGroup();
  settings.beginGroup("database.autosave");

  // Autosaver
  Autosaver* autos = new Autosaver(settings, &app);
  settings.endGroup();

  QObject::connect(db, &Database::OnDataChanged, autos,
				   &Autosaver::dataChanged);
  QObject::connect(autos, &Autosaver::OnAutosaveNeeded, db,
				   &Database::autosave);
  QObject::connect(autos, &Autosaver::OnAutosaveSkipp, db,
				   &Database::autosaveSkipped);
  autos->start();

  // Updater
  QSettings upd_settings(ini_file, QSettings::IniFormat);
  upd_settings.beginGroup("updater");
  Updater* updater = nullptr;
  if (upd_settings.value("enabled", false).toBool()) {
	updater = new Updater(upd_settings, *db, &app);
	qDebug("Updater    enabled");
  } else
	qDebug("Updater    disabled");

  APIController* api =
	  new APIController(colony_detector, plate_detector, nullptr, db, &app);
  if (updater)
	QObject::connect(api, &APIController::OnUpdateRequested, updater,
					 &Updater::TryUpdate);

  // Static file controller
  settings.beginGroup("files");
  StaticFileController* staticFileController = new StaticFileController(
	  settings, paths::docRoot().toSystemAbsolute(), &app);
  settings.endGroup();

  // SSL
  settings.beginGroup("ssl");
  QSslConfiguration* ssl = loadSslConfig(settings);

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

  // Calls stopLog when something throws an exception als
  // QCoreApplication::abouToQuit does not get emitted
  DtorCallback guard(
	  [db] {
		db->saveToFile();
		stopLog();
	  },
	  DtorCallback::CallType::kONLY_IN_EXC_CONTEXT);
  QObject::connect(&app, &QCoreApplication::aboutToQuit, stopLog);

  if (!ws_server->StartListen()) return 1;

  // send logging output to clients
  QObject::connect(Logger::instance(), &Logger::OnNewLine, ws_server,
				   &WsServer::NewDebugLine);

  return app.exec();
}

int main(int argc, char** argv) {
  int status;

  // Restarts the program when it exited with exitCodeSoftRestart()
  while (true) {
	qDebug("Starting");
	try {
	  status = start(argc, argv);
	} catch (std::exception const& e) {
	  qCritical("Exception: %s", e.what());
	} catch (...) {
	  qCritical("Exception: %s", "unknown");
	}

	if (status == exitCodeSoftRestart())
	  qDebug() << "Awaiting restart...";
	else
	  break;
  };

  return status;
}
