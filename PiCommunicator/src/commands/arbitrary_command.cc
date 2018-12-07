#include "commands/arbitrary_command.h"

#include <QJsonArray>
#include <QJsonObject>

namespace c3picko {
commands::ArbitraryCommand::ArbitraryCommand(QString command)
	: Command("printer/command", QJsonObject({{"command", command}}), // ugly
			  {204}, Command::Type::POST) {}

commands::ArbitraryCommand::ArbitraryCommand(QStringList commands)
	: Command("printer/command",
			  QJsonObject(
				  {{"commands", QJsonArray::fromStringList(commands)}}), // ugly
			  {204}, Command::Type::POST) {}
} // namespace c3picko
