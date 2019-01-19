#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QString>
#include "include/global.h"
#include "httplistener.h"
#include "include/requestmapper.h"
#include "include/ws_server.hpp"

using namespace stefanfrings;
using namespace c3picko;

/**
 * Search the configuration file.
 * Aborts the application if not found.
 * @return The valid filename
 */
QString searchConfigFile() {
	QFile file;
	file.setFileName(Etc() + "serverconfig.ini");

	QFileInfo info(file);
	qDebug() << info.absolutePath();
	if (file.exists()) {
		QString configFileName=QDir(file.fileName()).canonicalPath();
		qDebug("using config file %s", qPrintable(configFileName));
		return configFileName;
	}
	else {
		qFatal("config file not found");
	}
}

int main(int argc, char *argv[])
{
	Setup();
	QCoreApplication app(argc, argv);
	QString configFileName=searchConfigFile();

	// API Controller
	APIController* apiController = new APIController(&app);

	// Static file controller
	QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
	fileSettings->beginGroup("files");
	StaticFileController* staticFileController=new StaticFileController(fileSettings,&app);

	// HTTP server
	QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
	listenerSettings->beginGroup("listener");
	HttpListener listener(listenerSettings,new RequestMapper(staticFileController, apiController, &app),&app);

	// WS server
	WsServer _ws(apiController, &app);

	return app.exec();
}
