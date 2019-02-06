#include "httplistener.h"
#include "include/global.h"
#include "include/requestmapper.h"
#include "include/ws_server.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMetaObject>
#include <QObject>
#include <QSettings>
#include <QSslKey>
#include <QString>
#include <QtGlobal>
#include <csignal>

using namespace stefanfrings;
using namespace c3picko;

/*static void OnStatusOk(int, Command::Response* response)
{
	qDebug() << response->ToString();
}

static void OnStatusErr(QVariant status, Command::Response*) { qDebug() << qPrintable("Status error: " + status.toString()); }

static void OnNetworkErr(QString error) { qDebug() << qPrintable("Network error: " + error); }

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	c3picko::pi::OctoPrint printer("84.136.88.42", "CB46ACB8E7314CA5A41044F004DC6CE8");
	//c3picko::pi::commands::UploadFile cmd("M60", data::Location::LOCAL, "test3.gcode", false, false);

	//c3picko::pi::commands::UploadFile cmd("subsub", "", data::Location::LOCAL);
	//c3picko::pi::commands::DeleteFile cmd(data::Location::LOCAL, "test3.gcode");
	c3picko::pi::commands::GetAllFiles cmd(true);

	QObject::connect(&cmd, &Command::OnStatusOk, &OnStatusOk);
	QObject::connect(&cmd, &Command::OnStatusErr, &OnStatusErr);
	QObject::connect(&cmd, &Command::OnNetworkErr, &OnNetworkErr);
	QObject::connect(&cmd, SIGNAL(OnFinished()), &app, SLOT(quit()));

	printer.SendCommand(&cmd);
	return app.exec();
}*/

/**
 * Search the configuration file.
 * Aborts the application if not found.
 * @return The valid filename
 */
QString searchConfigFile()
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

void signalHandler(int id)
{
	// Check that the signal is valid, i doubt that the handler will ever get an invalid id, but otherwise a simple implemented strsignal
	// method would allow memory extraction of the hostmachine in the %s
	char const* sig_name = (id > 1 && id < 32) ? strsignal(id) : "<unknown>";

	// qFatal("Shutting down for Signal %s", sig_name);
	if (qApp)
	{
		qApp->exit(1); // FIXME this is not allowed see https://doc.qt.io/qt-5/unix-signals.html
	}
	// qFatal("Shutdown completed");
}

static WsServer* ws_ptr = nullptr;
void msg_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QString formated = qFormatLogMessage(type, context, msg);

	if (ws_ptr)
		QMetaObject::invokeMethod(ws_ptr, "NewDebugLine", Q_ARG(QString, formated));
	fprintf(stderr, "%s\n", formated.toLocal8Bit().constData());
};

int start(int argc, char** argv)
{
	qInstallMessageHandler(msg_handler);
	for (int i = 1; i < 7; ++i)
		std::signal(i, &signalHandler); // TODO check error SIG_ERR

	// FIXME memory managment
	Setup();
	QCoreApplication app(argc, argv);
	QString			 configFileName = searchConfigFile();

	Database db("database.json", &app);
	// API Controller
	APIController* apiController = new APIController(db, &app);

	// Static file controller
	QSettings* fileSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	fileSettings->beginGroup("files");
	StaticFileController* staticFileController = new StaticFileController(fileSettings, &app);

	QSslConfiguration* ssl				= nullptr;
	QSettings*		   listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	listenerSettings->beginGroup("listener");

	if (listenerSettings->value("ssl").toBool())
	{
		ssl = LoadSslConfig(listenerSettings);
		if (!ssl)
		{
			qCritical() << "SSL setup failed";
			return 1;
		}
		qDebug() << "SSL setup complete";
	}
	else
		qDebug() << "SSL disabled";

	// HTTP server
	HttpListener listener(listenerSettings, ssl, new RequestMapper(staticFileController, apiController, &app), &app);
	// WS server
	WsServer _ws(listenerSettings, ssl, apiController, &app);
	ws_ptr = &_ws;

	QObject::connect(&app, &QCoreApplication::aboutToQuit, [] {
		qInstallMessageHandler(nullptr); // reset message handlers
		ws_ptr = nullptr;				 // also dont redirect the console output to WsServer anymore
	});
	return app.exec();
}

int main(int argc, char** argv)
{
	int status = 0;

	do
	{
		status = start(argc, argv);
		qDebug() << "Server closed";
	} while (status == 123);

	return status;
}
