#ifndef FILE_TYPE_H
#define FILE_TYPE_H

#include <QString>

namespace c3picko
{
namespace commands
{
	namespace responses
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
	}
}
}

#endif // FILE_TYPE_H
