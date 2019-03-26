#include "include/algorithm_manager.h"
#include "include/algorithms/fluro1.h"
#include "include/algorithms/normal1.h"
#include "include/algorithms/plate1.h"
#include "include/api_controller.h"
#include "include/database.hpp"
#include "include/requestmapper.h"
#include "include/signal_daemon.h"
#include "include/ws_server.hpp"

#include "httplistener.h"
#include "staticfilecontroller.h"

#include <QFile>
#include <QGuiApplication>
#include <QThreadPool>
#include <QtGlobal>

using namespace stefanfrings;
using namespace c3picko;

static WsServer* ws_ptr = nullptr;

static QString textColor(QtMsgType type)
{
	switch (type)
	{
		case QtMsgType::QtInfoMsg:
			return "";
		case QtMsgType::QtDebugMsg:
			return "";
		case QtMsgType::QtWarningMsg:
			return "\x1B[33m"; // Orange
		case QtMsgType::QtCriticalMsg:
			return "\x1B[31m";		// Red
		case QtMsgType::QtFatalMsg: // is the same as QtSystemMsg
			return "\x1B[31;1m";	// Red, Fat
		default:
			Q_UNREACHABLE();
	}
}
static void msg_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QString message		= qFormatLogMessage(type, context, msg);
	QString time		= "[ " + QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate) + " ] ";
	QString color_begin = textColor(type), color_end = "\033[0m";

	if (ws_ptr)
		QMetaObject::invokeMethod(ws_ptr, "NewDebugLine", Q_ARG(QString, time + message));

		// TODO bad style to write everything to stderr
#ifdef Q_OS_LINUX
	fprintf(stderr, "%s%s%s%s\n", qPrintable(time), qPrintable(color_begin), qPrintable(message), qPrintable(color_end));
#else
	fprintf(stderr, "%s%s\n", qPrintable(time), qPrintable(message));
#endif
}

static int start(int argc, char** argv)
{
#ifdef Q_OS_LINUX
	qInstallMessageHandler(msg_handler);
#endif
	// We need a QGuiApplication for the QPdfWriter class from Report. Thats a bit unlucky
	// TODO search workaround
	QGuiApplication app(argc, argv);
	QString			ini_file = searchConfigFile(argc, argv);
	QSettings		settings(ini_file, QSettings::IniFormat);
	Setup(&app, ini_file, settings);

	QThreadPool* algo_pool = new QThreadPool(&app);
	algo_pool->setMaxThreadCount(qMin(1, QThread::idealThreadCount() / 2));
	AlgorithmManager* colony_detector = new AlgorithmManager(algo_pool, {new Normal1(), new Fluro1()}, &app);
	AlgorithmManager* plate_detector  = new AlgorithmManager(algo_pool, {new Plate1()}, &app);

	settings.beginGroup("database");
	Database*	  db  = new Database(settings, &app);
	APIController* api = new APIController(colony_detector, plate_detector, db, &app);

	// Static file controller
	settings.beginGroup("files");
	StaticFileController* staticFileController = new StaticFileController(settings, DocRoot().toSystemAbsolute(), &app);
	qInfo() << "DocRoot is" << DocRoot().toSystemAbsolute();

	// SSL
	settings.beginGroup("ssl");
	QSslConfiguration* ssl = LoadSslConfig(settings);

	// HTTP server
	QSettings* http_settings = new QSettings(ini_file, QSettings::IniFormat, &app);
	http_settings->beginGroup("http");
	new HttpListener(http_settings, ssl, new RequestMapper(staticFileController, &app), &app);

	// WS server
	QSettings* ws_settings = new QSettings(ini_file, QSettings::IniFormat, &app);
	ws_settings->beginGroup("websockets");
	WsServer* ws_server = new WsServer(*ws_settings, ssl, &app);

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

	// Restarts the program when it exited with exitCodeRestart()
	while (true)
	{
		qInfo("Starting");
		try
		{
			status = start(argc, argv);
		}
		catch (std::exception const& e)
		{
			qCritical("Exception: %s", e.what());
		}
		catch (...)
		{
			qCritical("Exception: %s", "unknown");
		}

		if (status == exitCodeRestart())
			qInfo() << "Awaiting restart...";
		else
			break;
	};

	return status;
}
