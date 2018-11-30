#include "commands/arbitrary_command.h"

#include <QJsonArray>
#include <QJsonObject>

namespace c3picko {

commands::ArbitraryCommand::ArbitraryCommand(QString command)
	: Command("printer/command", QJsonObject({{"command", command}}),  // ugly
			  {204}) {}

commands::ArbitraryCommand::ArbitraryCommand(QStringList commands)
	: Command("printer/command",
			  QJsonObject({{"commands",
							QJsonArray::fromStringList(commands)}}),  // ugly
			  {204}) {}

void commands::ArbitraryCommand::OnReplyFinished(QNetworkReply* reply) {
  Response* test = new Response();
  test->data1 = "asdf";
  test->data2 = 123;

  Command::CheckStatusCode(reply, test);
}
}
