#ifndef JSON_CONVERTABLE_H
#define JSON_CONVERTABLE_H

#include "include/global.h"
#include <QJsonObject>
#include <QJsonValueRef>

namespace c3picko {
	class JsonConvertable
	{
	public:
		virtual ~JsonConvertable() { }

		virtual void read(const QJsonObject&) = 0;
		virtual void write(QJsonObject&) const = 0;
	};
}

#endif // JSON_CONVERTABLE_H
