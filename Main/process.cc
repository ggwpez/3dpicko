#include "include/process.h"
#include <QCoreApplication>
#include <QProcess>
#include "include/exception.h"
#include "include/version.h"

namespace c3picko {
Process::Process(QStringList args, QObject* _parent)
    : QObject(_parent),
      git_(nullptr),
      args_(args),
      exec_dir_(ResourcePath::fromServerRelative("")) {}

Process::Process(QStringList args, const ResourcePath& exec_dir,
                 QObject* _parent)
    : QObject(_parent), git_(nullptr), args_(args), exec_dir_(exec_dir) {}

Process::~Process() {
  // git_ is deleted by this
}

Process* Process::gitClone(QString repo, ResourcePath const& into,
                           QStringList arguments) {
  QStringList args = {"clone", repo, into.toSystemAbsolute()};
  args.append(arguments);

  return new Process(args);
}

Process* Process::gitLog(const ResourcePath& repo, QStringList arguments) {
  QStringList args(arguments.empty()
                       ? QStringList{"--pretty=format:\"%H#%ad\"",
                                     "--date=rfc2822", "--max-count=5"}
                       : arguments);
  args.prepend("log");

  return new Process(args, repo);
}

Process* Process::gitCheckout(QString branch, const ResourcePath& repo) {
  return new Process({"checkout", branch}, repo);
}

void Process::start() {
  if (git_) throw Exception("Cant start process twice");

  QString git_path("git");
  git_ = new QProcess(this);
  git_->setWorkingDirectory(exec_dir_.toSystemAbsolute());
  git_->start(git_path, args_);

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
