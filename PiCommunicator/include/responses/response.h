#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <QJsonObject>
#include <QJsonDocument>

namespace c3picko
{
namespace pi
{
	namespace responses
	{
		struct Response
		{
			Response() = default;
			inline Response(QJsonObject) {}
			virtual inline QString ToString()
			{
				QJsonDocument doc(raw);
				return doc.toJson();
			}

			virtual inline ~Response() {}

		protected:
			QJsonObject raw;
		};
	} // namespace responses
} // namespace commands
} // namespace c3picko

#endif // RESPONSE_H_
