#pragma once

#include <QObject>
#include <QTimer>
#include "include/database.hpp"
#include "include/resource_path.h"

class QSettings;
namespace c3picko {
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
  Updater(QSettings const& settings, Database& db);
  ~Updater();

  static qint32 defaultInterval;
  static bool defaultEnabled;

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
  /**
   * @brief Searches for an update.
   */
  void search();

 private:
  Database& db_;
  ResourcePath repo_;
  qint32 interval_s_;
  QThread* worker_;
  QTimer* timer_;
};
}  // namespace c3picko
