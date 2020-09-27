/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef ARBITRARY_COMMAND_H_
#define ARBITRARY_COMMAND_H_

#include "PiCommunicator/command.h"

namespace d3picko {
namespace pi {
namespace commands {
/**
 * @brief Allows to send arbitraty commands to the printer.
 *
 * Use with care, this can break current jobs, since the commands are executed
 * immediately.
 */
class ArbitraryCommand : public Command {
  Q_OBJECT

  ArbitraryCommand() = delete;
  using Command::Command;

 public:
  static ArbitraryCommand* SingleCommand(QString command);
  static ArbitraryCommand* MultiCommand(QStringList commands);
};
}  // namespace commands
}  // namespace pi
}  // namespace d3picko

#endif	// ARBITRARY_COMMAND_H_
