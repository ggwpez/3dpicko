/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef OCTOPRINT_H_
#define OCTOPRINT_H_

#include <QNetworkAccessManager>
#include <QObject>
#include <QUrl>

#include "PiCommunicator/octoconfig.h"

namespace d3picko {
namespace pi {
class Command;
/**
 * @brief API for OctoPrint printer
 */
class OctoPrint : public QObject {
  Q_OBJECT

 public:
  OctoPrint(const OctoConfig& config, QObject* _parent = nullptr);

 signals:
  void OnResolved();

 public slots:
  void SendCommand(Command* cmd);

 private slots:
  void Resolve();

 private:
  OctoConfig config_;
  QNetworkAccessManager* network_;
  enum class ResolveStatus {
	UNRESOLVED,
	RESOLVING,
	RESOLVED,
	ERROR
  } resolve_status_;
  QUrl resolved_address_;
};
}  // namespace pi
}  // namespace d3picko

// TODO QNetworkAccessManager::networkAccessibleChanged()
#endif	// OCTOPRINT_H_
