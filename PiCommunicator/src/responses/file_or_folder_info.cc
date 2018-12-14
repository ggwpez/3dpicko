#include "responses/file_or_folder_info.h"
#include <QJsonArray>

namespace c3picko
{
namespace pi
{
	namespace responses
	{
		FileOrFolderInfo::FileOrFolderInfo(QJsonObject obj)
		{
			name	= obj["name"].toString();
			display = obj["display"].toString();
			path	= obj["path"].toString();
			type	= data::FileTypeFromString(obj["type"].toString());

			for (QJsonValue const& value : obj["typePath"].toArray())
				type_path.append(value.toString());

			if (type == data::FileType::FOLDER)
				ReadFolderMember(obj);
			else
				ReadFileMember(obj);
		}

		void FileOrFolderInfo::ReadFileMember(QJsonObject obj)
		{
			if (obj.contains("hash"))
				hash = obj["hash"].toString();

			if (obj.contains("size"))
				size = obj["size"].toInt();

			if (obj.contains("date"))
				date = obj["date"].toInt();

			origin = data::LocationFromString(obj["origin"].toString());

			if (obj.contains("refs"))
				refs = data::Reference(obj["refs"].toObject());

			if (obj.contains("gcodeAnalysis"))
				gcode_analysis = obj["gcodeAnalysis"].toString();
		}

		void FileOrFolderInfo::ReadFolderMember(QJsonObject obj)
		{
			if (obj.contains("children"))
			{
				for (QJsonValue const& value : obj["children"].toArray())
				{
					children.append(FileOrFolderInfo(value.toObject()));
				}
			}

			if (obj.contains("size"))
				size = obj["size"].toInt();
		}
	} // namespace responses
} // namespace commands
} // namespace c3picko
