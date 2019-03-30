#pragma once

#include <QObject>
#include <QTimer>
#include "include/resource_path.h"

class QSettings;
namespace c3picko {
struct Version {
  QString current_hash_, new_hash_;
};

class Updater : public QObject {
  Q_OBJECT
 public:
  Updater(QSettings const& settings);

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
   * The interval is specified in settings.updates.interval
   */
  void startSearch();
  /**
   * @brief Stops the search for updates.
   */
  void endSearch();
  /**
   * @brief Searches for an update.
   */
  void search();

 private:
  ResourcePath repo_;
  qint32 interval_s_;
  QTimer* timer_;
};
}  // namespace c3picko
