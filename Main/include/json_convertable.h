#ifndef JSON_CONVERTABLE_H
#define JSON_CONVERTABLE_H

#include "include/global.h"
#include <QJsonObject>
#include <QJsonValueRef>

namespace c3picko {
class JsonConvertable {
public:
  inline virtual ~JsonConvertable() {}

  virtual inline explicit operator QJsonObject() const {
    QJsonObject json;
    this->write(json);
    return json;
  }
  virtual void read(const QJsonObject &) = 0;
  virtual void write(QJsonObject &) const = 0;
};
} // namespace c3picko

#endif // JSON_CONVERTABLE_H
