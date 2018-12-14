#include "commands/arbitrary_command.h"

#include <QJsonArray>
#include <QJsonObject>

namespace c3picko
{
namespace pi
{
	namespace commands
	{
		ArbitraryCommand::ArbitraryCommand(QString command)
			: Command("printer/command", QJsonObject({{"command", command}}), // ugly
					  {204}, Command::HTTPType::POST)
		{
		}

		ArbitraryCommand::ArbitraryCommand(QStringList commands)
			: Command("printer/command", QJsonObject({{"commands", QJsonArray::fromStringList(commands)}}), // ugly
					  {204}, Command::HTTPType::POST)
		{
		}
	}
}
} // namespace c3picko
