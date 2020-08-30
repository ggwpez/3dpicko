/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/commands/arbitrary_command.h"

#include <QJsonArray>
#include <QJsonObject>

namespace c3picko {
namespace pi {
namespace commands {
ArbitraryCommand* ArbitraryCommand::SingleCommand(QString command) {
  return new ArbitraryCommand("printer/command",
							  QJsonObject({{"command", command}}),	// ugly
							  {204}, Command::HTTPType::POST);
}

ArbitraryCommand* ArbitraryCommand::MultiCommand(QStringList commands) {
  return new ArbitraryCommand(
	  "printer/command",
	  QJsonObject(
		  {{"commands", QJsonArray::fromStringList(commands)}}),  // ugly
	  {204}, Command::HTTPType::POST);
}
}  // namespace commands
}  // namespace pi
}  // namespace c3picko
