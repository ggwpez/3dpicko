/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Main/exception.h"

#include <string.h>

namespace d3picko {
Exception::Exception(const QString& what)
	: std::runtime_error(qPrintable(what)) {}

Exception::Exception(const QString& where, const QString& what)
	: std::runtime_error(qPrintable(where + ": " + what)) {}
}  // namespace d3picko
