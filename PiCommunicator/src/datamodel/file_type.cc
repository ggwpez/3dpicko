#include "datamodel/file_type.h"
#include <QString>

namespace c3picko
{
namespace pi
{
	namespace data
	{
		FileType FileTypeFromString(QString const& str)
		{
			if (str == "model")
				return FileType::MODEL;
			if (str == "machinecode")
				return FileType::MACHINECODE;
			if (str == "folder")
				return FileType::FOLDER;
			else
				throw std::runtime_error("Unknown FileType");
		}
	} // namespace responses
} // namespace commands
} // namespace c3picko