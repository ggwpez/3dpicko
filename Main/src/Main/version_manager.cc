/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Main/version_manager.h"

#include "GUI/database.h"
#include "Main/exception.h"
#include "Main/process.h"

namespace d3picko {
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
	qDebug().nospace().noquote() << "Installation complete (id=" << id << ")";
	db_.installedVersions().push_back(id);
	linkVersion(id);

	// if (db_.installedVersions().size() > max_interesting_)
	// remOldestVersion(db_.installedVersions().front());
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
	return (void)qWarning("Can only remove the oldest version");
  if (db_.installedVersions().size() == 1)
	return (void)qWarning("There must be at least one version of interest");
  if (currentVersion().id() == id)
	return (void)qWarning("Cant remove current version");
  if (selected_ == id) return (void)qWarning("Cant remove selected version");

  qDebug() << "Removing old version" << id;
  // Is there a process active right now?
  if (processes_.find(id) != processes_.end())	// Kill the process
	processes_.at(id)->kill();

  ResourcePath build(working_dir_ + "builds/" + id);
  if (!QDir(build.toSystemAbsolute()).removeRecursively())
	qWarning() << "Could not remove directory" << build.toSystemAbsolute();

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

	qDebug() << "Installing version" << id;
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

void VersionManager::qmakeAmdMakeVersion(Version::ID id) {
  ResourcePath new_build_dir(working_dir_ + "builds/" + id);

  // Check for the build directory and maybe copy an old build in there, to
  // speed up make.
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
	  /*if (!copyRecursively(old_build_dir.toSystemAbsolute(),
	  new_build_dir.toSystemAbsolute()))
	  {
									  // If it did not work, delete it and
	  create a new empty directory
									  // (again).
									  qWarning() << "Cold not copy build
	  directories"; QDir(new_build_dir.toSystemAbsolute()).removeRecursively();
	  // dont check return code

									  if
	  (!QDir().mkdir(new_build_dir.toSystemAbsolute())) return emit
	  OnInstallError("qmake (setup phase)", "Cant create build dir" +
	  new_build_dir.toSystemAbsolute());
	  }
	  else
									  qDebug() << "Copied build dir";*/
	  // Dont copy the building dir, otherwise the git hash from qmake is not
	  // correct anymore
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
  connect(make, &Process::OnSuccess,
		  [this, id]() { emit this->OnInstalled(id); });

  registerProcess(id, make);
  make->start();
}

void VersionManager::linkVersion(Version::ID id) {
  ResourcePath s_main(working_dir_ + "main"),
	  s_lqua(working_dir_ + "libquazip.so.1");
  QString t_main("builds/" + id + "/Main/Main"),
	  t_lqua("builds/" + id + "/depend/quazip/quazip/libquazip.so.1");

  Process* ln_main(Process::ln(t_main, s_main));
  Process* ln_lqua(Process::ln(t_lqua, s_lqua));

  connect(ln_main, &Process::OnFailure, [this, id](QString output) {
	emit this->OnInstallError("ln", output);
  });
  connect(ln_main, &Process::OnFinished, ln_main, &Process::deleteLater);
  connect(ln_main, &Process::OnSuccess, [this, id, ln_lqua]() {
	registerProcess(id, ln_lqua);
	ln_lqua->start();
  });

  connect(ln_lqua, &Process::OnFailure, [this, id](QString output) {
	emit this->OnInstallError("ln", output);
  });
  connect(ln_lqua, &Process::OnFinished, ln_lqua, &Process::deleteLater);
  connect(ln_lqua, &Process::OnSuccess, [this, id] {
	selected_ = id;
	emit this->OnSwitched(id);
  });

  registerProcess(id, ln_main);
  ln_main->start();
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
}  // namespace d3picko
