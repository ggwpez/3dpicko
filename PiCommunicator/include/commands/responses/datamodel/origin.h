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

		Origin OriginFromString(QString str);
	}
}
}

#endif // ORIGIN_H
