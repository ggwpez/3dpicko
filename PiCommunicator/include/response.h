#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <QJsonObject>

namespace c3picko
{
namespace pi
{
	namespace responses
	{
		struct Response
		{
			Response() = default;
			Response(QJsonObject const& obj);
			virtual QString ToString();

			virtual inline ~Response() {}

		  protected:
			QJsonObject raw;
		};
	} // namespace responses
} // namespace commands
} // namespace c3picko

#endif // RESPONSE_H_
