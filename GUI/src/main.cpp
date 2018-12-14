#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QString>
#include "httplistener.h"
#include "requestmapper.h"

using namespace stefanfrings;

/**
 * Search the configuration file.
 * Aborts the application if not found.
 * @return The valid filename
 */
QString searchConfigFile() {
	QString binDir=QCoreApplication::applicationDirPath();
	QString appName=QCoreApplication::applicationName();
	QFile file;
    file.setFileName(binDir+"/../../etc/webapp1.ini");

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
	QCoreApplication app(argc, argv);
	QString configFileName=searchConfigFile();

	// Session store
	QSettings* sessionSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
	sessionSettings->beginGroup("sessions");
	RequestMapper::sessionStore=new HttpSessionStore(sessionSettings,&app);

	// Static file controller
	QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
	fileSettings->beginGroup("files");
	RequestMapper::staticFileController=new StaticFileController(fileSettings,&app);

	// HTTP server
	QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
	listenerSettings->beginGroup("listener");
	HttpListener listener(listenerSettings,new RequestMapper(&app),&app);

	return app.exec();
}
