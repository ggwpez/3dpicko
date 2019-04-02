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

void VersionManager::addVersion(Version::ID id) {
  if (db_.versionsOI().exists(id))
    return (void)qCritical("New versions should not be already of interest");

  if (db_.versionsOI().size() >= max_interesting_) {
    QList<Version> vois(db_.versionsOI().entries().values());
    std::sort(vois.begin(), vois.end(), [](Version const& a, Version const& b) {
      return (a.date() < b.date());
    });  // ORDER BY would be nice… TODO?

    while (db_.versionsOI().size() >= max_interesting_) {
      remVersion(vois.back().id());
    }
  }
  // add
}

void VersionManager::remVersion(Version::ID id) {
  if (!db_.versionsOI().exists(id))
    throw Exception("Can only remove version of interest");

  // Is there a process active right now?
  if (current_.second && current_.first == id) {
    current_.second->kill();
  }
}

void VersionManager::checkout(Version::ID id) {
  Version& version(db_.versions().get(id));

  if (version.state() == Version::State::CLONED ||
      version.state() == Version::State::CHECK_OUT_ERROR) {
    transition(version, Version::State::MARKED_FOR_CHECKOUT);
    Process* git(enterProcess(
        id, Process::gitCheckout(repo_branch_, working_dir_ + "source/" + id)));

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
    connect(git, &Process::OnFinished,
            [id, this]() { this->leaveProcess(id); });

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

Process* VersionManager::enterProcess(Version::ID id, Process* proc) {
  if (current_.second) {
    qCritical("More than one process is not permitted right now");
    return nullptr;
  }

  current_ = {id, proc};
  return proc;
}

void VersionManager::leaveProcess(Version::ID id) {
  if (!current_.second) return (void)qCritical("Process to cancel was null");
  if (current_.first != id)
    return (void)qCritical("Process to cancel had wrong id");

  current_.second->deleteLater();
  current_ = {Version::ID(), nullptr};
}
}  // namespace c3picko
