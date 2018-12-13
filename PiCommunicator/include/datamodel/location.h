#ifndef ORIGIN_H
#define ORIGIN_H

#include <QString>

namespace c3picko
{
namespace pi
{
	namespace data
	{
		///
		/// [API](http://docs.octoprint.org/en/master/api/datamodel.html#files)
		///
		enum class Location : char
		{
			LOCAL,
			SD_CARD
		};

		Location LocationFromString(QString str);
		QString ToString(Location val);
	} // namespace data
} // namespace commands
} // namespace c3picko

#endif // ORIGIN_H
