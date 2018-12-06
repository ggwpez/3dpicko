#ifndef FILE_INFORMATION_H_
#define FILE_INFORMATION_H_

#include "commands/responses/datamodel/file_type.h"
#include "commands/responses/datamodel/gcode_analysis.h"
#include "commands/responses/datamodel/origin.h"
#include "commands/responses/datamodel/reference.h"
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVector>

namespace c3picko
{
namespace commands
{
	namespace responses
	{
		/**
		 * @brief @link
		 * http://docs.octoprint.org/en/master/api/datamodel.html#sec-api-datamodel-files-file
		 * @endlink
		 * TODO there may be additional properties see link above
		 */
		struct FileOrFolderInfo
		{

			FileOrFolderInfo() = default;
			FileOrFolderInfo(QJsonObject obj);

			QString		name, display, path;
			FileType	type;
			QStringList type_path;

			// File specific
			QString		  hash;
			qint64		  date;
			Origin		  origin;
			Reference	 refs;
			GCodeAnalysis gcode_analysis;

			// Folder specific
			QVector<FileOrFolderInfo> children;

			// Shared
			qint64 size;

		  private:
			void ReadFileMember(QJsonObject obj);
			void ReadFolderMember(QJsonObject obj);
		};
	} // namespace responses
} // namespace commands
} // namespace c3picko

#endif // FILE_INFORMATION_H_
