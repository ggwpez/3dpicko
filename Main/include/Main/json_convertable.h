#ifndef JSON_CONVERTABLE_H
#define JSON_CONVERTABLE_H

#include "Main/global.h"
#include <QJsonObject>
#include <QJsonValueRef>

namespace c3picko
{
class JsonConvertable
{
  public:
	virtual ~JsonConvertable();

	virtual explicit operator QJsonObject() const;
	virtual void	 read(const QJsonObject&)  = 0;
	virtual void	 write(QJsonObject&) const = 0;
};
} // namespace c3picko

#endif // JSON_CONVERTABLE_H
