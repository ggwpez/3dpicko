/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Main/json_convertable.h"

namespace c3picko {
JsonConvertable::~JsonConvertable() {}

c3picko::JsonConvertable::operator QJsonObject() const {
  QJsonObject json;
  this->write(json);
  return json;
}
}  // namespace c3picko
