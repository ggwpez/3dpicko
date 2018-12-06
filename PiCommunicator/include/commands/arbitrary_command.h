#ifndef ARBITRARY_COMMAND_H_
#define ARBITRARY_COMMAND_H_

#include "command.h"

namespace c3picko
{
namespace commands
{
	class ArbitraryCommand : public Command
	{
		Q_OBJECT

	  public:
		ArbitraryCommand(QString command);
		ArbitraryCommand(QStringList commands);
	};
} // namespace commands
} // namespace c3picko

#endif // ARBITRARY_COMMAND_H_
