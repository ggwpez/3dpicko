#ifndef JSON_CONVERTABLE_H
#define JSON_CONVERTABLE_H

#include <QJsonObject>
#include <QJsonValueRef>
#include "include/global.h"

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
}  // namespace c3picko

#endif  // JSON_CONVERTABLE_H
