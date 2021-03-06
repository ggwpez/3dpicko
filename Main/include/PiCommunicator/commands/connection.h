/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef CONNECT_H_
#define CONNECT_H_

#include "PiCommunicator/command.h"
#include "PiCommunicator/responses/connection_info.h"

namespace d3picko {
namespace pi {
namespace commands {
/**
 * @brief Allows to send arbitraty commands to the printer.
 *
 * Use with care, this can break current jobs, since the commands are executed
 * immediately.
 */
class Connection : public Command {
  Q_OBJECT

  Connection() = delete;
  using Command::Command;

 public:
  typedef responses::ConnectionInfo Response;

  static Connection* GetInfo();
  static Connection* Connect(QString port, qint32 baudrate = -1,
							 QString printer_profile = "", bool save = false,
							 bool autoconnect = false);
  static Connection* Disconnect();
  static Connection* FakeAck();

 public slots:
  virtual void OnReplyFinished(QNetworkReply* reply) override;

 private:
  static QJsonObject CreateConnectJson(QString port, qint32 baudrate,
									   QString printer_profile, bool safe,
									   bool autoconnect);
};
}  // namespace commands
}  // namespace pi
}  // namespace d3picko

#endif	// CONNECT_H
