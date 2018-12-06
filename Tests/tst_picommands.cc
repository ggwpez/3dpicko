#include <QFile>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QtTest>

#include "commands/get_all_files.h"
#include "commands/upload_file.h"
#include "octoprint.h"

using namespace c3picko;
class PiCommands : public QObject
{
	Q_OBJECT

  public:
	PiCommands();

  signals:
	void done();

  private slots:
	void initTestCase();
	void GetAllFiles();
	void UploadFile1();
	void cleanupTestCase();

	void OnStatusOk(int status, Command::Response* response);
	void OnStatusErr(QVariant status, Command::Response* response);
	void OnNetworkErr(QString error);

  private:
	QProcess netcat;

	QVector<QString> const file_names_;
};

PiCommands::PiCommands() : netcat(this), file_names_{"whistle_v2.gcode", "whistle_.gco", "folderA"}
{
	qputenv("QTEST_FUNCTION_TIMEOUT", "5000");
}

void PiCommands::initTestCase()
{
	QString filename = QFINDTESTDATA("retrive_response.html");
	netcat.start("/bin/nc -l 8080 -w1");
	QVERIFY(netcat.waitForStarted());

	QFile file(filename);
	QVERIFY(file.open(QIODevice::ReadOnly));
	QByteArray data = file.readAll();

	netcat.write(data);
	netcat.closeWriteChannel();
}

void PiCommands::GetAllFiles()
{
	int				   argc = 0;
	QCoreApplication   app(argc, nullptr);
	c3picko::OctoPrint printer("127.0.0.1:8080", "<KEY>");

	c3picko::commands::GetAllFiles cmd(true);

	QObject::connect(&cmd, &Command::OnStatusOk, this, &PiCommands::OnStatusOk);
	QObject::connect(&cmd, &Command::OnStatusErr, this, &PiCommands::OnStatusErr);
	QObject::connect(&cmd, &Command::OnNetworkErr, this, &PiCommands::OnNetworkErr);
	QObject::connect(this, &PiCommands::done, &app, &QCoreApplication::quit);

	printer.SendCommand(&cmd);
	app.exec();
}

void PiCommands::UploadFile1()
{
	c3picko::commands::UploadFile cmd("subfolder", "folder/");

	QString filename = QFINDTESTDATA("upload_response1.http");
	QString data	 = cmd.GetPostData();

	QFile file(filename);
	QVERIFY(file.open(QIODevice::ReadOnly));
	QByteArray expect = file.readAll();

	QCOMPARE(data, expect);
}

void PiCommands::cleanupTestCase() { netcat.waitForFinished(1000); }

void PiCommands::OnStatusOk(int, Command::Response* response)
{
	auto* rep = static_cast<commands::GetAllFiles::Response*>(response);

	QVERIFY(rep->files.size() == file_names_.size());
	for (int i = 0; i < rep->files.size(); ++i)
		QCOMPARE(rep->files[i].name, file_names_[i]);

	emit done();
}

void PiCommands::OnStatusErr(QVariant status, Command::Response*)
{
	emit done();
	QFAIL(qPrintable("Status error: " + status.toString()));
}

void PiCommands::OnNetworkErr(QString error)
{
	emit done();
	QFAIL(qPrintable("Network error: " + error));
}

QTEST_APPLESS_MAIN(PiCommands)

#include "tst_picommands.moc"
