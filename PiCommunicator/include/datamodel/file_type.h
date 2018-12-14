#ifndef FILE_TYPE_H
#define FILE_TYPE_H

#include <QString>

namespace c3picko
{
namespace pi
{
	namespace data
	{
		///
		/// [API](http://docs.octoprint.org/en/master/api/datamodel.html#file-information)
		///
		enum class FileType : char
		{
			NONE,
			MODEL,
			MACHINECODE,
			FOLDER
		};

		FileType FileTypeFromString(QString str);
	} // namespace data
} // namespace commands
} // namespace c3picko

#endif // FILE_TYPE_H
