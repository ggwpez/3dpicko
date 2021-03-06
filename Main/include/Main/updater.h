/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QMutex>
#include <QObject>
#include <QTimer>

#include "GUI/database.h"
#include "Main/resource_path.h"

class QSettings;
namespace d3picko {
class Process;
class VersionManager;
/**
 * @brief Searches for new commits on a specific branch on the given git
 * repository and compares then with all known versions. When a new version is
 * detected, it will be downloaded and build. Then updateAvailable is emitted.
 *
 * Updater uses its own thread to perform synchronous operations on his child
 * processes to ensure that the timer does not trigger faster than we can
 * process the triggers.
 */
class Updater : public QObject {
  Q_OBJECT
 public:
  Updater(QSettings const& settings, Database& db, QObject* _parent = nullptr);
  ~Updater();

  static qint32 defaultInterval;
  static bool defaultEnabled;

  ResourcePath sourceDir() const;
  ResourcePath buildDir(Version::ID = "") const;

  Process* clone();
  Process* checkout(Version::ID hash);

  VersionManager* mng() const;

 signals:
  /**
   * @brief Emitted when a new update is available.
   * @param How many updates were found.
   * @param Last version.
   */
  void updateAvailable(QString version);
  void noUpdateAvailable();

 private slots:
  /**
   * @brief Starts the periodic search for updates.
   * The interval is specified in settings.updater.interval
   */
  void startSearch();
  /**
   * @brief Stops the search for updates.
   */
  void stopSearch();

 public:
  /**
   * @brief Searches for an update.
   */
  void TryUpdate();

 private:
  // Git callbacks
  void logSuccess(QString);
  void logFailure(QString);

 private:
  Database& db_;
  ResourcePath working_dir_;
  qint32 interval_s_;
  QTimer* timer_;
  QString repo_url_;
  QString repo_branch_;
  VersionManager* mng_;
  QMutex mtx_;
};
}  // namespace d3picko
