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

#ifdef Q_OS_UNIX
#include "include/signal_daemon.h"
#endif

using namespace stefanfrings;
using namespace c3picko;

/**
 * Search the configuration file.
 * Aborts the application if not found.
 * @return The valid filename
 */
static QString searchConfigFile()
{
	QFile file;
	file.setFileName(Etc() + "serverconfig.ini");

	QFileInfo info(file);
	if (file.exists())
	{
		QString configFileName = QDir(file.fileName()).canonicalPath();
		qDebug("using config file %s", qPrintable(configFileName));
		return configFileName;
	}
	else
	{
		qFatal("config file not found");
		qApp->exit(1);
	}
	return "";
}

static void setupSignals(QCoreApplication* app)
{
#ifdef Q_OS_UNIX
	SignalDaemon* sigwatch = new SignalDaemon(app);
	sigwatch->registerSignals({SIGABRT, SIGALRM, SIGFPE, SIGHUP, SIGILL, SIGINT, SIGPIPE, SIGQUIT, SIGSEGV, SIGTERM, SIGUSR1, SIGUSR2});

	QObject::connect(sigwatch, &SignalDaemon::OnSignal, [&app](int signum) {
		qWarning("Shutdown by signal: %s", ::strsignal(signum));
		app->quit();
	});
#else
	qDebug() << "UNIX Signal Setup skipped";
#endif
}

static WsServer* ws_ptr = nullptr;
static void		 msg_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QString formated = qFormatLogMessage(type, context, msg);
	QString now		 = QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate);
	formated		 = "[ " + now + " ] " + formated;

	if (ws_ptr)
		QMetaObject::invokeMethod(ws_ptr, "NewDebugLine", Q_ARG(QString, formated));

	fprintf(stderr, "%s\n", qPrintable(formated));
}

static int start(int argc, char** argv)
{
#ifdef Q_OS_LINUX
	qInstallMessageHandler(msg_handler);
#endif
	// FIXME memory managment
	Setup();
	QCoreApplication app(argc, argv);
	setupSignals(&app);
	QString configFileName = searchConfigFile();

	QThreadPool* algo_pool = new QThreadPool(&app);
	algo_pool->setMaxThreadCount(qMin(1, QThread::idealThreadCount() / 2));
	AlgorithmManager* detector = new AlgorithmManager(algo_pool, {new Normal1(), new Fluro1()}, &app);

	Database*	  db  = new Database("database.json", &app);
	APIController* api = new APIController(detector, db, &app);

	// Static file controller
	QSettings* fileSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	fileSettings->beginGroup("files");
	StaticFileController* staticFileController = new StaticFileController(fileSettings, &app);

	// SSL
	QSettings* ssl_settings = new QSettings(configFileName, QSettings::IniFormat, &app);
	ssl_settings->beginGroup("ssl");

	QSslConfiguration* ssl = nullptr;
	if (ssl_settings->value("enabled", false).toBool())
	{
		ssl = LoadSslConfig(ssl_settings);
		if (!ssl)
		{
			qCritical() << "SSL setup failed";
			return 1;
		}
		qDebug() << "SSL setup completed";
	}
	else
		qDebug() << "SSL disabled";

	// HTTP server
	QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	listenerSettings->beginGroup("http");
	new HttpListener(listenerSettings, ssl, new RequestMapper(staticFileController, &app), &app);
	// WS server
	QSettings* ws_settings = new QSettings(configFileName, QSettings::IniFormat, &app);
	ws_settings->beginGroup("websockets");
	WsServer* ws_server = new WsServer(ws_settings, ssl, &app);

	QObject::connect(ws_server, &WsServer::OnRequest, api, &APIController::request);
	QObject::connect(api, &APIController::toClient, ws_server, &WsServer::ToClient);
	QObject::connect(api, &APIController::toAll, ws_server, &WsServer::ToAll);
	QObject::connect(api, &APIController::toAllExClient, ws_server, &WsServer::ToAllExClient);

	QObject::connect(&app, &QCoreApplication::aboutToQuit, [db] {
		db->saveToFile();				 // save database
		qInstallMessageHandler(nullptr); // reset message handlers
		ws_ptr = nullptr;				 // also dont redirect the console output to WsServer anymore
	});

	if (!ws_server->StartListen())
		return 1;
	else
		ws_ptr = ws_server;

	return app.exec();
}

int main(int argc, char** argv)
{
	int status;

	// Restarts the program when it exited with exitRestart()
	while (true)
	{
		qDebug("Starting");
		status = start(argc, argv);

		if (status == exitRestart())
			qDebug() << "Awaiting restart...";
		else
			break;
	};

	return status;
}
