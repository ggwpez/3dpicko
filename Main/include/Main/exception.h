/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QException>

namespace d3picko {
class Exception : public std::runtime_error {
 public:
  Exception(const QString& what);
  Exception(QString const& where, QString const& what);
};
}  // namespace d3picko
