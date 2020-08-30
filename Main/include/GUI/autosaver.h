/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QObject>

class QTimer;
class QSettings;
namespace d3picko {
/**
 * @brief Can be used to send autosave notifications to all types derivates of
 * QObject .
 *
 * Class is thrad-safe .
 */
class Autosaver : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief Autosaver
   */
  Autosaver(QSettings const& settings, QObject* _parent = nullptr);

 signals:
  /**
   * @brief Will be emitted when an autosave should be dont for all listeners .
   */
  void OnAutosaveNeeded();
  /**
   * @brief Will be emitted when no autosave is needed, just for logging manners
   * .
   */
  void OnAutosaveSkipp();

 public slots:
  void start();
  void stop();

  void dataChanged();

 private slots:
  void resetDataChanged();
  void tick();

 private:
  /**
   * @brief Check interval in milliseconds .
   */
  qint32 interval_ms_;
  bool enabled_;
  bool data_changed_;
  QTimer* timer_;
};
}  // namespace d3picko
