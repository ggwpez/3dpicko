#include "datamodel/location.h"
#include <QString>

namespace c3picko
{
namespace pi
{
	namespace data
	{
		Location LocationFromString(const QString& str)
		{
			if (str == "local")
				return Location::LOCAL;
			if (str == "sdcard")
				return Location::SD_CARD;
			else
				throw std::runtime_error("Origin type unknown");
		}

		QString ToString(Location val)
		{
			if (val == Location::LOCAL)
				return "local";
			else
				return "sdcard";
		}

	} // namespace responses
} // namespace commands
} // namespace c3picko
