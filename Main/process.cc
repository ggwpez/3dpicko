#include "include/process.h"
#include <QCoreApplication>
#include <QProcess>
#include "include/exception.h"
#include "include/version.h"

namespace c3picko {
Process::Process(QString binary, QStringList args, QObject* _parent)
    : QObject(_parent),
      binary_(binary),
      git_(nullptr),
      args_(args),
      exec_dir_(ResourcePath::fromServerRelative("")) {}

Process::Process(QString binary, QStringList args, const ResourcePath& exec_dir,
                 QObject* _parent)
    : QObject(_parent),
      binary_(binary),
      git_(nullptr),
      args_(args),
      exec_dir_(exec_dir) {}

Process::~Process() {
  // git_ is deleted by this
}

Process* Process::gitClone(QString repo, ResourcePath const& into,
                           QStringList arguments) {
  QStringList args = {"clone", repo, into.toSystemAbsolute()};
  args.append(arguments);

  return new Process("git", args);
}

Process* Process::gitLog(const ResourcePath& repo, QStringList arguments) {
  QStringList args(arguments);
  args.prepend("log");

  return new Process("git", args, repo);
}

Process* Process::gitFech(const ResourcePath& repo, QStringList arguments) {
  QStringList args(arguments);
  args.prepend("fetch");

  return new Process("git", args, repo);
}

Process* Process::gitCheckout(QString branch, const ResourcePath& repo) {
  return new Process("git", {"checkout", branch}, repo);
}

Process* Process::qmake(const ResourcePath& repo, const ResourcePath& source) {
  return new Process("qmake", {source.toSystemAbsolute()}, repo);
}

void Process::start() {
  if (git_) throw Exception("Cant start process twice");

  QString git_path(binary_);
  git_ = new QProcess(this);
  git_->setWorkingDirectory(exec_dir_.toSystemAbsolute());
  git_->start(git_path, args_);
  qDebug() << git_path << args_;

  if (!git_->waitForStarted(3000))
    emit OnFailure("Could not start (path=" + git_path +
                   "): " + git_->errorString());
  emit OnStarted();
  if (!git_->waitForFinished(getSubprocessTimeoutMs()))
    emit OnFailure("Timed out (path=" + git_path + ", max_time=" +
                   QString::number(getSubprocessTimeoutMs()) + "ms)");
  else if (git_->exitStatus() != QProcess::NormalExit)
    emit OnFailure("Crashed (path=" + git_path +
                   "): " + git_->readAllStandardError());
  else if (git_->exitCode() != 0)
    emit OnFailure("Error (path=" + git_path +
                   "): " + git_->readAllStandardError());
  else {
    QString output = git_->readAllStandardOutput().replace(
        "\"", "");  // FIXME why is this needed?
    emit OnSuccess(output);
  }

  emit OnFinished();
}

void Process::kill() { git_->kill(); }
}  // namespace c3picko
