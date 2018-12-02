#include <QCoreApplication>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QtTest>

#include "commands/get_all_files.h"
#include "octoprint.h"

using namespace c3picko;
class Json : public QObject {
  Q_OBJECT

public:
  Json();

signals:
  void done();

private Q_SLOTS:
  void initTestCase();
  void testcase1();
  void cleanUpTestCase();

  void OnStatusOk(int status, Command::Response *response);
  void OnStatusErr(QVariant status, Command::Response *response);
  void OnNetworkErr(QString error);

private:
  QProcess netcat;

  QVector<QString> const file_names_;
};

Json::Json()
    : netcat(this), file_names_{"whistle_v2.gcode", "whistle_.gco", "folderA"} {

}

void Json::initTestCase() {
  QString filename = QFINDTESTDATA("retrive_response.html");
  netcat.start("/bin/nc -l 8080 -w1");
  QVERIFY(netcat.waitForStarted());

  QFile file(filename);

  QVERIFY(file.open(QIODevice::ReadOnly));
  QByteArray data = file.readAll();

  netcat.write(data);
  netcat.closeWriteChannel();
}

void Json::testcase1() {
  int argc = 0;
  QCoreApplication a(argc, nullptr);
  c3picko::OctoPrint printer("127.0.0.1:8080", "<KEY>");

  c3picko::commands::GetAllFiles cmd(true);

  QObject::connect(&cmd, &Command::OnStatusOk, this, &Json::OnStatusOk);
  QObject::connect(&cmd, &Command::OnStatusErr, this, &Json::OnStatusErr);
  QObject::connect(&cmd, &Command::OnNetworkErr, this, &Json::OnNetworkErr);
  QObject::connect(this, &Json::done, &a, &QCoreApplication::quit);

  printer.SendCommand(&cmd);
  a.exec();
}

void Json::cleanUpTestCase() { netcat.waitForFinished(1000); }

void Json::OnStatusOk(int status, Command::Response *response) {
  auto *rep = static_cast<commands::GetAllFiles::Response *>(response);

  QVERIFY(rep->files.size() == file_names_.size());
  for (int i = 0; i < rep->files.size(); ++i)
    QCOMPARE(rep->files[i].name, file_names_[i]);

  emit done();
}

void Json::OnStatusErr(QVariant status, Command::Response *response) {
  qDebug() << "Status err: " << status;
  emit done();
}

void Json::OnNetworkErr(QString error) {
  qDebug() << "Network error: " << error;
  emit done();
}

QTEST_MAIN(Json)

#include "tst_json.moc"
