#pragma once

#include <QObject>
#include <QQueue>

#include "Main/version.h"

namespace c3picko {
class Process;
class Database;
/**
 * @brief This class MUST run in Main thread, it is not designed for
 * multithreading. Also the database class cant handle that.
 */
class VersionManager : public QObject {
  Q_OBJECT
 public:
  VersionManager(ResourcePath working_dir, QString repo_url,
				 QString repo_branch, Database& db, QObject* _parent = nullptr);

  ResourcePath sourcePath() const;
  Version::ID selected() const;

 public slots:
  void installVersion(Version::ID);
  void remOldestVersion(Version::ID);

 private slots:
  /**
   * @brief Installes the next version from to_be_installed_ .
   */
  void installNext();
  void checkoutAndQmakeVersion(Version::ID);
  void qmakeAmdMakeVersion(Version::ID);
  void makeVersion(Version::ID);
  /**
   * @brief Replaces the symlink that points to the current version
   */
  void linkVersion(Version::ID);

  void transition(Version&, Version::State state);
  void registerProcess(Version::ID id, Process* proc);
  void unregisterProcess(Version::ID id);

  void checkoutRepo(Version::ID);

 signals:
  void OnTransition(Version::ID, Version::State old, Version::State now);

  void OnInstallBegin(Version::ID);
  void OnInstalled(Version::ID);
  void OnInstallError(Version::ID, QString error);
  void OnUnInstalled(Version::ID);
  /**
   * @brief Emitted when the version was switched and a restart should take
   * place to commence the changes.
   * @param now New version.
   */
  void OnSwitched(Version::ID now);

 private:
  Database& db_;
  QString repo_url_;
  QString repo_branch_;
  ResourcePath working_dir_;
  /**
   * @brief The version that will be used after restarting the server.
   */
  Version::ID selected_;
  /**
   * @brief Maximum number of interesting versions.
   * Must be at least 1.
   */
  qint32 max_interesting_ = 3;
  /**
   * @brief the top element is the currently running
   */
  std::map<Version::ID, Process*> processes_;
  /**
   * @brief Queue of versions that should be installed.
   * Only one version can be installed at a time, thats why this queue keeps
   * track of whats next.
   */
  QQueue<Version::ID> to_be_installed_;
  // TODO bad style
  int make_retries_ = 0;
  int const max_make_retries_ = 3;
};
}  // namespace c3picko
