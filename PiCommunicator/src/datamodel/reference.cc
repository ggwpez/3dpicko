#include "datamodel/reference.h"

namespace c3picko
{
namespace pi
{
	namespace data
	{
		Reference::Reference(QJsonObject obj)
		{
			resource = obj["resource"].toString();

			if (obj.contains("download"))
				download = obj["download"].toString();
			if (obj.contains("model"))
				download = obj["model"].toString();
		}
	} // namespace responses
} // namespace commands
} // namespace c3picko
