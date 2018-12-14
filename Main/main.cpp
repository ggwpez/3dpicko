#include "include/commands/all.h"
#include "include/octoprint.h"
#include <QCoreApplication>

using namespace std;
using namespace c3picko;
using namespace c3picko::pi;

static void OnStatusOk(int, Command::Response* response)
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
}
