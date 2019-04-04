#include "include/version_manager.h"
#include "include/database.hpp"
#include "include/exception.h"
#include "include/process.h"

namespace c3picko {
VersionManager::VersionManager(ResourcePath working_dir, QString repo_url,
                               QString repo_branch, Database& db,
                               QObject* _parent)
    : QObject(_parent),
      db_(db),
      repo_url_(repo_url),
      repo_branch_(repo_branch),
      working_dir_(working_dir),
      max_interesting_(0) {
  connect(this, &VersionManager::OnInstallError, this,
          [this](Version::ID id, QString error) {
            if (!to_be_installed_.size() || to_be_installed_.head() != id)
              qWarning("OnInstallError error");
            else
              to_be_installed_.dequeue();
            qDebug() << "Installation failed (id=" << id << "):" << error;
          });
  connect(this, &VersionManager::OnInstalled, this, [this](Version::ID id) {
    if (!to_be_installed_.size() || to_be_installed_.head() != id)
      qWarning("OnInstallError error");
    else
      to_be_installed_.dequeue();
    qDebug() << "Installation complete (id=" << id << ")";
  });
}

ResourcePath VersionManager::sourcePath() const {
  return working_dir_ + "source/";
}

void VersionManager::installVersion(Version::ID id) {
  to_be_installed_.enqueue(id);
  installNext();
}

void VersionManager::remOldestVersion(Version::ID id) {
  if (db_.installedVersions().front() != id)
    throw Exception("Can only remove the oldest version");
  if (db_.installedVersions().size() == 1)
    throw Exception("There must be at least one version of interest");

  qDebug() << "Removing old version" << id;
  // Is there a process active right now?
  if (processes_.find(id) != processes_.end())  // Kill the process
    processes_.at(id)->kill();

  // TODO remove build dir
  emit OnUnInstalled(id);
  db_.installedVersions().pop_front();
}

void VersionManager::installNext() {
  if (!to_be_installed_.isEmpty()) {
    Version::ID id(to_be_installed_.head());
    emit OnInstallBegin(id);

    if (db_.installedVersions().contains(id))
      return emit OnInstallError(id, "Cant install version twice");

    qDebug() << "Selecting version" << id;
    checkoutAndQmakeVersion(id);
  }
}

void VersionManager::checkoutAndQmakeVersion(Version::ID id) {
  Process* git(Process::gitCheckout(id, working_dir_ + "source/"));

  connect(git, &Process::OnStarted,
          [id]() { qDebug() << "Checking out" << id << "..."; });
  connect(git, &Process::OnSuccess, [this, id]() { qmakeAmdMakeVersion(id); });
  connect(git, &Process::OnFailure, [this, id](QString output) {
    emit this->OnInstallError(id, "git checkout: " + output);
  });
  connect(git, &Process::OnFinished, git, &Process::deleteLater);

  registerProcess(id, git);
  git->start();
}

void VersionManager::qmakeAmdMakeVersion(Version::ID id) {
  ResourcePath build_dir(working_dir_ + "builds/" + id);

  if (!QDir(build_dir.toSystemAbsolute()).exists())
    if (!QDir().mkdir(build_dir.toSystemAbsolute()))
      return emit OnInstallError(
          "qmake (setup phase)",
          "Cant create build dir" + build_dir.toSystemAbsolute());

  Process* qmake(Process::qmake(build_dir, sourcePath()));

  connect(qmake, &Process::OnFailure, [this, id](QString output) {
    emit this->OnInstallError("qmake", output);
  });
  connect(qmake, &Process::OnFinished, qmake, &Process::deleteLater);
  connect(qmake, &Process::OnSuccess, [this, id]() { this->makeVersion(id); });

  registerProcess(id, qmake);
  qmake->start();
}

void VersionManager::makeVersion(Version::ID id) {
  ResourcePath build_dir(working_dir_ + "builds/" + id);
  Process* make(Process::make(build_dir));

  connect(make, &Process::OnFailure, [this, id](QString output) {
    emit this->OnInstallError("make", output);
  });
  connect(make, &Process::OnFinished, make, &Process::deleteLater);
  connect(make, &Process::OnSuccess, [this, id]() { this->linkVersion(id); });

  registerProcess(id, make);
  make->start();
}

void VersionManager::linkVersion(Version::ID id) {
  ResourcePath link(working_dir_ + "main");
  QString binary("builds/" + id + "/Main/Main");

  // TODO we would need a transaction here, since when we first delete the link
  // and than create a new one, the application could crash after the deletion
  // and the new link would not be created. To emulate a transaction we use an
  // ln instance that does not listen to HUP
  Process* ln(Process::ln(binary, link));

  connect(ln, &Process::OnFailure, [this, id](QString output) {
    emit this->OnInstallError("ln", output);
  });
  connect(ln, &Process::OnFinished, ln, &Process::deleteLater);
  connect(ln, &Process::OnSuccess, [this, id] { emit this->OnInstalled(id); });

  registerProcess(id, ln);
  ln->start();
}

void VersionManager::checkoutRepo(Version::ID id) {
  Version& version(db_.versions().get(id));

  if (version.state() == Version::State::CLONED ||
      version.state() == Version::State::CHECK_OUT_ERROR) {
    transition(version, Version::State::MARKED_FOR_CHECKOUT);
    Process* git(
        Process::gitCheckout(repo_branch_, working_dir_ + "source/" + id));

    connect(git, &Process::OnStarted, [&version, this]() {
      transition(version, Version::State::CHECKING_OUT);
    });
    connect(git, &Process::OnSuccess, [&version, this](QString output) {
      transition(version, Version::State::CHECKED_OUT);
      qDebug() << output;
    });
    connect(git, &Process::OnFailure, [&version, this](QString output) {
      transition(version, Version::State::CHECK_OUT_ERROR);
      qDebug() << output;
    });
    connect(git, &Process::OnFinished, git, &Process::deleteLater);

    registerProcess(id, git);
    git->start();
  } else {
    qCritical() << "Cant check out in current state (state=" +
                       toString(version.state()) + ")";
    version.setState(Version::State::CHECK_OUT_ERROR);
  }
}

void VersionManager::transition(Version& version, Version::State state) {
  if (state == version.state())
    qWarning() << "Ignoring identity transition on version" << version.id();
  else {
    Version::State old(version.state());
    version.setState(state);
    qDebug() << "State transition: " << toString(old) << "=>"
             << toString(state);
    emit OnTransition(version.id(), old, state);
  }
}

void VersionManager::registerProcess(Version::ID id, Process* proc) {
  processes_[id] = proc;
  connect(proc, &Process::OnFinished, this,
          [this, id]() { this->unregisterProcess(id); });
}

void VersionManager::unregisterProcess(Version::ID id) {
  processes_.erase(id);
  if (to_be_installed_.size() && to_be_installed_.head() == id)
    emit OnInstallError(id, "Canceled");
}
}  // namespace c3picko
