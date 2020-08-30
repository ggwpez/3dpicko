/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef APIKEY_H_
#define APIKEY_H_

#include <QString>

#include "Main/marshalling.h"

namespace c3picko {
namespace pi {
struct ApiKey {
  ApiKey(QString const& key);

  QString key() const;

 private:
  QString key_;
};
}  // namespace pi
MAKE_MARSHALLABLE(pi::ApiKey);
}  // namespace c3picko

#endif	// APIKEY_H_
