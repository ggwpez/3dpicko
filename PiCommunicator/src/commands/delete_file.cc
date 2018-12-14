#include "commands/delete_file.h"

namespace c3picko
{
namespace pi
{
	namespace commands
	{
		DeleteFile::DeleteFile(data::Location location, QString path)
			: Command("files/" + data::ToString(location) + '/' + path, {204}, HTTPType::DELETE)
		{
		}
	}
}
}
