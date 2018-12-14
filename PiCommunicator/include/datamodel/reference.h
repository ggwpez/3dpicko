#ifndef REFERENCE_H
#define REFERENCE_H

#include <QJsonObject>
#include <QUrl>

namespace c3picko
{
namespace pi
{
	namespace data
	{
		/**
		 * @brief
		 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#references)
		 */
		struct Reference
		{
			Reference() = default;
			Reference(QJsonObject obj);

			QUrl resource, download, model;
		};
	} // namespace data
} // namespace commands
} // namespace c3picko

#endif // REFERENCE_H
