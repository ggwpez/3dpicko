#pragma once

#include "include/global.h"
#include <QJsonObject>
#include <QJsonValueRef>

namespace c3picko {
	class JsonConstructable
	{
	public:
		inline JsonConstructable(QJsonObject const&) { }
		inline virtual ~JsonConstructable() { }

		virtual inline explicit operator QJsonObject() const
		{
			QJsonObject json;
			this->write(json);
			return json;
		}
		//virtual void read(const QJsonObject&) = 0;
		virtual void write(QJsonObject&) const = 0;
	};
}
