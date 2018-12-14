#ifndef FILE_INFORMATION_H_
#define FILE_INFORMATION_H_

#include "datamodel/file_type.h"
#include "datamodel/gcode_analysis.h"
#include "datamodel/location.h"
#include "datamodel/reference.h"
#include "responses/response.h"
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVector>

namespace c3picko
{
namespace pi
{
	namespace responses
	{
		/**
		 * @brief
		 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#sec-api-datamodel-files-file)
		 * TODO there may be additional properties see link above
		 */
		struct FileOrFolderInfo : public Response
		{
			FileOrFolderInfo() = default;
			FileOrFolderInfo(QJsonObject obj);

			QString		   name, display, path;
			data::FileType type;
			QStringList	type_path;

			// File specific
			QString				hash;
			qint64				date;
			data::Location		origin;
			data::Reference		refs;
			data::GCodeAnalysis gcode_analysis;

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
