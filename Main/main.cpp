#include "httplistener.h"
#include "include/api_controller.h"
#include "include/command.h"
#include "include/commands/arbitrary_command.h"
#include "include/global.h"
#include "include/octoprint.h"
#include "include/requestmapper.h"
#include "include/signal_daemon.h"
#include "include/ws_server.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMetaObject>
#include <QObject>
#include <QSettings>
#include <QSslKey>
#include <QString>
#include <QThreadPool>
#include <QtGlobal>
#include <csignal>
#include <signal.h>

using namespace stefanfrings;
using namespace c3picko;
using namespace c3picko::pi;
using namespace c3picko::pi::commands;

/**
 * Search the configuration file.
 * Aborts the application if not found.
 * @return The valid filename
 */
static QString searchConfigFile() {
  QFile file;
  file.setFileName(Etc() + "serverconfig.ini");

  QFileInfo info(file);
  if (file.exists()) {
    QString configFileName = QDir(file.fileName()).canonicalPath();
    qDebug("using config file %s", qPrintable(configFileName));
    return configFileName;
  } else {
    qFatal("config file not found");
    qApp->exit(1);
  }
  return "";
}

static void setupSignals(QCoreApplication *app) {
  SignalDaemon *sigwatch = new SignalDaemon(app);
  sigwatch->registerSignals({SIGABRT, SIGALRM, SIGFPE, SIGHUP, SIGILL, SIGINT,
                             SIGPIPE, SIGQUIT, SIGSEGV, SIGTERM, SIGUSR1,
                             SIGUSR2});

  QObject::connect(sigwatch, &SignalDaemon::OnSignal, [&app](int signum) {
    qWarning("Shutdown by signal: %s", ::strsignal(signum));
    app->quit();
  });
}

static WsServer *ws_ptr = nullptr;
static void msg_handler(QtMsgType type, const QMessageLogContext &context,
                        const QString &msg) {
  QString formated = qFormatLogMessage(type, context, msg);

  if (ws_ptr)
    QMetaObject::invokeMethod(ws_ptr, "NewDebugLine", Q_ARG(QString, formated));
  QString now = QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate);

  fprintf(stderr, "[ %s ]  %s\n", qPrintable(now), qPrintable(formated));
};

static int start(int argc, char **argv) {
  qInstallMessageHandler(msg_handler);
  // FIXME memory managment
  Setup();
  QCoreApplication app(argc, argv);
  setupSignals(&app);
  QString configFileName = searchConfigFile();

  Database *db = new Database("database.json", &app);
  APIController *api =
      new APIController(QThreadPool::globalInstance(), db, &app);

  // Static file controller
  QSettings *fileSettings =
      new QSettings(configFileName, QSettings::IniFormat, &app);
  fileSettings->beginGroup("files");
  StaticFileController *staticFileController =
      new StaticFileController(fileSettings, &app);

  QSslConfiguration *ssl = nullptr;
  QSettings *listenerSettings =
      new QSettings(configFileName, QSettings::IniFormat, &app);
  listenerSettings->beginGroup("listener");

  if (listenerSettings->value("ssl").toBool()) {
    ssl = LoadSslConfig(listenerSettings);
    if (!ssl) {
      qCritical() << "SSL setup failed";
      return 1;
    }
    qDebug() << "SSL setup completed";
  } else
    qDebug() << "SSL disabled";

  // HTTP server
  HttpListener listener(listenerSettings, ssl,
                        new RequestMapper(staticFileController, &app), &app);
  // WS server
  WsServer *ws_server = new WsServer(listenerSettings, ssl, &app);
  ws_ptr = ws_server;

  QObject::connect(ws_server, &WsServer::OnRequest, api,
                   &APIController::request);
  QObject::connect(api, &APIController::toClient, ws_server,
                   &WsServer::ToClient);
  QObject::connect(api, &APIController::toAll, ws_server, &WsServer::ToAll);
  QObject::connect(api, &APIController::toAllExClient, ws_server,
                   &WsServer::ToAllExClient);

  QObject::connect(&app, &QCoreApplication::aboutToQuit, [] {
    qInstallMessageHandler(nullptr); // reset message handlers
    ws_ptr =
        nullptr; // also dont redirect the console output to WsServer anymore
  });

  ws_server->StartListen();

  return app.exec();
}

int main(int argc, char **argv) {
  int status = 0;

  do {
    status = start(argc, argv);
    qDebug() << "Server closed";
  } while (status == 123);

  return status;
}
