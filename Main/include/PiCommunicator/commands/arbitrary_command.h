#ifndef ARBITRARY_COMMAND_H_
#define ARBITRARY_COMMAND_H_

#include "PiCommunicator/command.h"

namespace c3picko {
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
  static ArbitraryCommand *SingleCommand(QString command);
  static ArbitraryCommand *MultiCommand(QStringList commands);
};
}  // namespace commands
}  // namespace pi
}  // namespace c3picko

#endif  // ARBITRARY_COMMAND_H_
