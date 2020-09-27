/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/commands/connection.h"

namespace d3picko {
namespace pi {
namespace commands {
Connection* Connection::GetInfo() {
  return new Connection{"connection", {200}, HTTPType::GET};
}

Connection* Connection::Connect(QString port, qint32 baudrate,
								QString printer_profile, bool save,
								bool autoconnect) {
  return new Connection{
	  "connection",
	  CreateConnectJson(port, baudrate, printer_profile, save, autoconnect),
	  {204},
	  HTTPType::POST};
}

Connection* Connection::Disconnect() {
  return new Connection{
	  "connection", {{"command", "disconnect"}}, {204}, HTTPType::POST};
}

Connection* Connection::FakeAck() {
  return new Connection{
	  "connection", {{"command", "fake_ack"}}, {204}, HTTPType::POST};
}

void Connection::OnReplyFinished(QNetworkReply* reply) {
  if (type_ == HTTPType::GET)  // Are we waiting for a response ?
	CheckStatusCodeAndResponse<Response>(reply);
  else if (type_ == HTTPType::POST)	 // No response in POST
	CheckStatusCode(reply, nullptr);
  else
	Q_UNREACHABLE();
}

QJsonObject Connection::CreateConnectJson(QString port, qint32 baudrate,
										  QString printer_profile, bool save,
										  bool autoconnect) {
  QJsonObject ret;
  ret["command"] = "connect";

  if (port.length()) ret["port"] = port;
  if (baudrate > 0) ret["baudrate"] = baudrate;
  if (printer_profile.length()) ret["printerProfile"] = printer_profile;
  if (save) ret["save"] = true;
  if (autoconnect) ret["autoconnect"] = true;

  return ret;
}
}  // namespace commands
}  // namespace pi
}  // namespace d3picko
