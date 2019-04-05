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
      selected_(currentVersion().id()),
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
      qWarning("OnInstall error");
    else
      to_be_installed_.dequeue();
    qDebug() << "Installation complete (id=" << id << ")";
    db_.installedVersions().push_back(id);
  });
}

ResourcePath VersionManager::sourcePath() const {
  return working_dir_ + "source/";
}

Version::ID VersionManager::selected() const { return selected_; }

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

  connect(git, &Process::OnSuccess, [this, id]() { qmakeAmdMakeVersion(id); });
  connect(git, &Process::OnFailure, [this, id](QString output) {
    emit this->OnInstallError(id, "git checkout: " + output);
  });
  connect(git, &Process::OnFinished, git, &Process::deleteLater);

  registerProcess(id, git);
  git->start();
}

// https://forum.qt.io/topic/59245/is-there-any-api-to-recursively-copy-a-directory-and-all-it-s-sub-dirs-and-files/3
// at 05.04.2019
static bool copyRecursively(QString sourceFolder, QString destFolder) {
  bool success = false;
  QDir sourceDir(sourceFolder), destDir(destFolder);

  if (!sourceDir.exists()) {
    qWarning() << "Could not find source dirctory" << sourceFolder;
    return false;
  }

  if (!destDir.exists()) {
    qWarning() << "Could not find destination dirctory" << destFolder;
    return false;
  }

  // TODO try QDirIterator
  QStringList files = sourceDir.entryList(QDir::Files);
  for (int i = 0; i < files.count(); i++) {
    QString srcName = sourceFolder + QDir::separator() + files[i];
    QString destName = destFolder + QDir::separator() + files[i];
    success = QFile::copy(srcName, destName);
    if (!success) return false;
  }

  files.clear();
  files = sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (int i = 0; i < files.count(); i++) {
    QString srcName = sourceFolder + QDir::separator() + files[i];
    QString destName = destFolder + QDir::separator() + files[i];
    success = copyRecursively(srcName, destName);
    if (!success) return false;
  }

  return true;
}

void VersionManager::qmakeAmdMakeVersion(Version::ID id) {
  ResourcePath new_build_dir(working_dir_ + "builds/" + id);

  // Check for the build directory and maybe copy an old build in there, to
  // speed up make
  if (!QDir(new_build_dir.toSystemAbsolute()).exists()) {
    if (!QDir().mkdir(new_build_dir.toSystemAbsolute()))
      return emit OnInstallError(
          "qmake (setup phase)",
          "Cant create build dir" + new_build_dir.toSystemAbsolute());

    // Can we get an older build?
    auto it(std::find(db_.installedVersions().begin(),
                      db_.installedVersions().end(), id));
    if (it != db_.installedVersions().begin()) {
      --it;
      ResourcePath old_build_dir(working_dir_ + "builds/" + *it);
      // Copy older build to new directory
      if (!copyRecursively(old_build_dir.toSystemAbsolute(),
                           new_build_dir.toSystemAbsolute())) {
        // If it did not work, delete it and create a new empty directory
        // (again).
        qWarning() << "Cold not copy build directories";
        QDir(new_build_dir.toSystemAbsolute())
            .removeRecursively();  // dont check return code

        if (!QDir().mkdir(new_build_dir.toSystemAbsolute()))
          return emit OnInstallError(
              "qmake (setup phase)",
              "Cant create build dir" + new_build_dir.toSystemAbsolute());
      } else
        qDebug() << "Copied build dir";
    }
  }

  Process* qmake(Process::qmake(new_build_dir, sourcePath()));

  connect(qmake, &Process::OnFailure, [this, id](QString output) {
    emit this->OnInstallError("qmake", output);
  });
  connect(qmake, &Process::OnFinished, qmake, &Process::deleteLater);
  connect(qmake, &Process::OnSuccess, [this, id]() {
    make_retries_ = 0;
    this->makeVersion(id);
  });

  registerProcess(id, qmake);
  qmake->start();
}

void VersionManager::makeVersion(Version::ID id) {
  ResourcePath build_dir(working_dir_ + "builds/" + id);
  Process* make(Process::make(build_dir));

  connect(make, &Process::OnFailure, [this, id](QString output) {
    if (output.contains("internal compiler error")) {
      if (++make_retries_ < max_make_retries_) {
        qWarning() << "Compiler crashed (segmentation fault),"
                   << (max_make_retries_ - make_retries_) << "tries left";
        this->makeVersion(id);
      } else
        emit this->OnInstallError(
            id, "Compiler crashed (segmentation fault) too often");
    } else
      emit this->OnInstallError("make", output);
  });
  connect(make, &Process::OnFinished, make, &Process::deleteLater);
  connect(make, &Process::OnSuccess, [this, id]() {
    emit this->OnInstalled(id);
    this->linkVersion(id);
  });

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
  // FIXME also update the link to libquazip
  connect(ln, &Process::OnFailure, [this, id](QString output) {
    emit this->OnInstallError("ln", output);
  });
  connect(ln, &Process::OnFinished, ln, &Process::deleteLater);
  connect(ln, &Process::OnSuccess, [this, id] {
    selected_ = id;
    emit this->OnSwitched(id);
  });

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
