#ifndef ORIGIN_H
#define ORIGIN_H

#include <QString>

namespace c3picko
{
namespace commands
{
	namespace responses
	{
		///
		/// [API](http://docs.octoprint.org/en/master/api/datamodel.html#files)
		///
		enum class Origin : char
		{
			NONE,
			LOCAL,
			SD_CARD
		};

		inline Origin OriginFromString(QString str)
		{
			if (str == "local")
				return Origin::LOCAL;
			if (str == "sdcard")
				return Origin::SD_CARD;
			else
				throw std::runtime_error("Origin type unknown");
		}
	}
}
}

#endif // ORIGIN_H
