#include "include/commands/all.h"
#include "include/octoprint.h"
#include <QCoreApplication>

using namespace c3picko;
using namespace c3picko::pi;
using namespace c3picko::pi::commands;

static void OnStatusOk(int, Command::Response* response)
{
	if (response)
		qDebug() << "response: " << qPrintable(response->ToString());
}

static void OnStatusErr(QVariant status, Command::Response*) { qDebug() << qPrintable("Status error: " + status.toString()); }

static void OnNetworkErr(QString error) { qDebug() << qPrintable("Network error: " + error); }

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	OctoPrint printer("192.168.2.100", "2CCEF1563DA648D3962431A69431295C");
	Command*  cmd = UploadFile::CreateFile("M60\nM33\nG X10 Y20 Z10", data::Location::LOCAL, "new.gcode");

	QObject::connect(cmd, &Command::OnStatusOk, &OnStatusOk);
	QObject::connect(cmd, &Command::OnStatusErr, &OnStatusErr);
	QObject::connect(cmd, &Command::OnNetworkErr, &OnNetworkErr);

	QObject::connect(cmd, SIGNAL(OnFinished()), cmd, SLOT(deleteLater()));
	QObject::connect(cmd, SIGNAL(OnFinished()), &app, SLOT(quit()));

	printer.SendCommand(cmd);
	return app.exec();
}
