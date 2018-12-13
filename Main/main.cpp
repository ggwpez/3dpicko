#include "include/commands/get_all_files.h"
#include "include/commands/upload_file.h"
#include "include/octoprint.h"
#include <QCoreApplication>

using namespace std;
using namespace c3picko;
using namespace c3picko::pi;

static void OnStatusOk(int, Command::Response* response)
{
	auto* rep = static_cast<commands::GetAllFiles::Response*>(response);

	for (int i = 0; i < rep->files.size(); ++i)
		qDebug() << (rep->files[i].type == data::FileType::FOLDER ? "-" : " ") << rep->files[i].name;
}

static void OnStatusErr(QVariant status, Command::Response*) { qDebug() << qPrintable("Status error: " + status.toString()); }

static void OnNetworkErr(QString error) { qDebug() << qPrintable("Network error: " + error); }

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	c3picko::pi::OctoPrint printer("192.168.2.16", "7E364BB8D1514181861F6021763D7415");
	// c3picko::commands::UploadFile cmd("Hello World", "local", "new.txt");
	c3picko::pi::commands::GetAllFiles cmd(true);

	QObject::connect(&cmd, &Command::OnStatusOk, &OnStatusOk);
	QObject::connect(&cmd, &Command::OnStatusErr, &OnStatusErr);
	QObject::connect(&cmd, &Command::OnNetworkErr, &OnNetworkErr);

	printer.SendCommand(&cmd);
	return app.exec();
}
