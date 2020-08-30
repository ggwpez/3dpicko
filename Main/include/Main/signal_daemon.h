/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <signal.h>

#include <QObject>
#include <QSet>
class QSocketNotifier;

namespace d3picko {
/**
 * @brief Class for intercepting UNIX signals and making them accessible for Qt.
 * See https://doc.qt.io/qt-5/unix-signals.html
 */
class SignalDaemon : public QObject {
  Q_OBJECT

 public:
  SignalDaemon(QObject* _parent = nullptr);

  // Unix signal handler
  int registerSignal(int signal);
  int registerSignals(QVector<int> signal);

  static void signalProxy(int signal);

 private slots:
  void socketNotify(int socket_fd);

 signals:
  void OnSignal(int signal);

 private:
  static int socketpair_[2];

  QSocketNotifier* notifier_;
  QSet<int> registered_;
};
}  // namespace d3picko
