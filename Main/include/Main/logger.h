/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QMutex>
#include <QObject>

namespace d3picko {
/**
 * @brief Singelton
 */
class Logger : public QObject {
  Q_OBJECT
  Logger(int backlog_length = 1000);

 public:
  static Logger* instance();
  void setBacklogLength(int backlog_length);

  /**
   * @brief Get latest messages.
   * @param count	How many latest messages . < 0 means all .
   */
  QStringList getLast(int count = -1) const;

 protected:
  void normalise();

 public slots:
  void log(QString);
  void log(QStringList);

 signals:
  void OnNewLine(QString);

 private:
  int backlog_length_;
  QStringList backlog_;
  QMutex lock_;
};
}  // namespace d3picko
