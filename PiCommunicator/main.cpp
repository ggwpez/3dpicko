#include <QCoreApplication>
#include <QDebug>
#include "commands/get_all_files.h"
#include "octoprint.h"

using namespace c3picko;
using namespace c3picko::commands;

static void OnStatusOk(int status, Command::Response* answer) {
  auto* rep = static_cast<GetAllFiles::Response*>(answer);
  qDebug() << "Status ok: " << status;

  for (auto const& file : rep->files)
	  qDebug() << "File: " << file.name;
}

static void OnStatusErr(QVariant status, Command::Response* answer) {
  qDebug() << "Status err: " << status;
}

static void OnNetworkErr(QString error) {
  qDebug() << "Network error: " << error;
}

int main(int argc, char** argv) {
  QCoreApplication a(argc, argv);
  c3picko::OctoPrint printer("127.0.0.1:8080", "<KEY>");

  c3picko::commands::GetAllFiles cmd(true);

  QObject::connect(&cmd, &Command::OnStatusOk, &OnStatusOk);
  QObject::connect(&cmd, &Command::OnStatusErr, &OnStatusErr);
  QObject::connect(&cmd, &Command::OnNetworkErr, &OnNetworkErr);

  printer.SendCommand(&cmd);

  return a.exec();
}
