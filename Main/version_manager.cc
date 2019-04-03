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
      max_interesting_(0) {}

ResourcePath VersionManager::sourcePath() const {
  return working_dir_ + "source/";
}

void VersionManager::addVersion(Version::ID id) {
  if (db_.versionsOI().contains(id))
    return (void)qCritical("New versions should not already be of interest");

  if (db_.versionsOI().size() >= max_interesting_ && db_.versionsOI().size()) {
    QVector<Version::ID> vois(db_.versionsOI());

    while (db_.versionsOI().size() >= max_interesting_) {
      remOldestVersion(vois.back());
      vois.pop_back();
    }
  }

  qDebug() << "Selecting version" << id;
  qDebug() << "Working dir:" << working_dir_.toSystemAbsolute();
  db_.versionsOI().push_front(id);
  checkoutAndQmakeVersion(id);
}

void VersionManager::remOldestVersion(Version::ID id) {
  if (db_.versionsOI().front() != id)
    throw Exception("Can only remove the oldest version");
  if (db_.versionsOI().size() == 1)
    throw Exception("There must be at least one version of interest");

  qDebug() << "Removing old version" << id;
  // Is there a process active right now?
  if (processes_.find(id) != processes_.end()) {
    // Kill the process
    processes_.at(id)->kill();
  }
  db_.versionsOI().pop_front();
}

void VersionManager::checkoutAndQmakeVersion(Version::ID id) {
  Process* git(Process::gitCheckout(id, working_dir_ + "source/"));

  connect(git, &Process::OnStarted,
          [id]() { qDebug() << "Checking out" << id << "..."; });
  connect(git, &Process::OnSuccess, [this, id]() { qmakeAmdMakeVersion(id); });
  connect(git, &Process::OnFailure,
          [](QString output) { qDebug() << "Check out error:" << output; });
  connect(git, &Process::OnFinished, git, &Process::deleteLater);

  registerProcess(id, git);
  git->start();
}

void VersionManager::qmakeAmdMakeVersion(Version::ID id) {
  ResourcePath build_dir(working_dir_ + "builds/" + id);

  if (!QDir().mkdir(build_dir.toSystemAbsolute()))
    return (void)(qCritical()
                  << "Cant create build dir" << build_dir.toSystemAbsolute());

  Process* qmake(Process::qmake(build_dir, sourcePath()));

  connect(qmake, &Process::OnFailure,
          [](QString output) { qDebug() << output; });
  connect(qmake, &Process::OnFinished, qmake, &Process::deleteLater);
  connect(qmake, &Process::OnSuccess, [this, id](QString output) {
    qDebug() << output;

    this->makeVersion(id);
  });

  registerProcess(id, qmake);
  qmake->start();
}

void VersionManager::makeVersion(Version::ID id) {
  ResourcePath build_dir(working_dir_ + "builds/" + id);
  Process* make(Process::make(build_dir));

  connect(make, &Process::OnFailure,
          [](QString output) { qDebug() << output; });
  connect(make, &Process::OnFinished, make, &Process::deleteLater);
  connect(make, &Process::OnSuccess, [id](QString output) {
    qDebug().noquote() << output;

    qWarning() << "Version build" << id;
  });

  registerProcess(id, make);
  make->start();
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
  qDebug() << "Registering process:" << id;
}

void VersionManager::unregisterProcess(Version::ID id) {
  processes_.erase(id);
  qDebug() << "Unregistering process:" << id;
}
}  // namespace c3picko
