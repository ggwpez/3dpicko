/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <QJsonObject>

namespace d3picko {
namespace pi {
namespace responses {
struct Response {
  Response() = default;
  Response(QJsonObject const& obj);
  virtual QString ToString();

  virtual inline ~Response() {}

 protected:
  QJsonObject raw;
};
}  // namespace responses
}  // namespace pi
}  // namespace d3picko

#endif	// RESPONSE_H_
