#include <QCoreApplication>
#include <QDebug>
#include "commands/arbitrary_command.h"
#include "octoprint.h"

using namespace c3picko;
using namespace c3picko::commands;

static void OnStatusOk(int status, Command::Response* answer) {
  auto* rep = static_cast<ArbitraryCommand::Reply*>(answer);
  qDebug() << "Status ok: " << status << "\n"
           << "Data1: " << rep->data1 << " data2: " << rep->data2;
}

static void OnStatusErr(QVariant status, Command::Response* answer) {
  qDebug() << "Status err: " << status;
}

static void OnNetworkErr(QString error) {
  qDebug() << "Network error: " << error;
}

int main(int argc, char** argv) {
  QCoreApplication a(argc, argv);
  c3picko::OctoPrint printer("localhost:8080", "<KEY>");

  c3picko::commands::ArbitraryCommand cmd({"M17", "M106 S0"});

  QObject::connect(&cmd, &Command::OnStatusOk, &OnStatusOk);
  QObject::connect(&cmd, &Command::OnStatusErr, &OnStatusErr);
  QObject::connect(&cmd, &Command::OnNetworkErr, &OnNetworkErr);

  printer.SendCommand(&cmd);

  return a.exec();
}
