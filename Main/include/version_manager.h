#pragma once

#include <QObject>
#include <queue>
#include "include/version.h"

namespace c3picko {
class Process;
class Database;
class VersionManager : public QObject {
  Q_OBJECT
 public:
  VersionManager(ResourcePath working_dir, QString repo_url,
                 QString repo_branch, Database& db, QObject* _parent = nullptr);

 public slots:
  void addVersion(Version::ID);
  void remOldestVersion(Version::ID);

 private slots:
  void transition(Version&, Version::State state);
  void registerProcess(Version::ID id, Process* proc);
  void unregisterProcess(Version::ID id);

  void checkout(Version::ID);

 signals:
  void OnTransition(Version::ID, Version::State old, Version::State now);

 private:
  Database& db_;
  QString repo_url_;
  QString repo_branch_;
  ResourcePath working_dir_;
  /**
   * @brief Maximum number of interesting versions.
   */
  qint32 max_interesting_ = 5;
  /**
   * @brief the top element is the currently running
   */
  std::map<Version::ID, Process*> processes_;
};
}  // namespace c3picko
