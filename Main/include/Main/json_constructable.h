#pragma once

#include <QJsonObject>
#include <QJsonValueRef>

#include "Main/global.h"

namespace c3picko {
class JsonConstructable {
 public:
  inline JsonConstructable(QJsonObject const&) {}
  inline virtual ~JsonConstructable() {}

  virtual inline explicit operator QJsonObject() const {
	QJsonObject json;
	this->write(json);
	return json;
  }

  virtual void write(QJsonObject&) const = 0;
};
}  // namespace c3picko
