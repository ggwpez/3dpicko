/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QJsonObject>
#include <QJsonValueRef>

#include "Main/global.h"

namespace d3picko {
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
}  // namespace d3picko
