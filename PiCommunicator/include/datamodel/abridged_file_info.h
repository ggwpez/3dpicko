#ifndef ABRIDGED_FILE_INFORMATION_H_
#define ABRIDGED_FILE_INFORMATION_H_

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
	namespace data
	{
		/**
		 * @brief
		 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#sec-api-datamodel-files-file)
		 */
		struct AbridgedFileInfo
		{
			AbridgedFileInfo() = default;
			AbridgedFileInfo(QJsonObject obj);

			QString			name, display, path;
			data::Location  origin;
			data::Reference ref;
		};
	} // namespace data
} // namespace commands
} // namespace c3picko

#endif // ABRIDGED_FILE_INFORMATION_H_
