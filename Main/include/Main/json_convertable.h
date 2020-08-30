/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef JSON_CONVERTABLE_H
#define JSON_CONVERTABLE_H

#include <QJsonObject>
#include <QJsonValueRef>

#include "Main/global.h"

namespace c3picko {
class JsonConvertable {
 public:
  virtual ~JsonConvertable();

  virtual explicit operator QJsonObject() const;
  virtual void read(const QJsonObject&) = 0;
  virtual void write(QJsonObject&) const = 0;
};
}  // namespace c3picko

#endif	// JSON_CONVERTABLE_H
