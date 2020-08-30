/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 flopicko flobotic@protonmail.com
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include "Main/marshalling.h"

namespace c3picko {
/**
 * @brief Type of the plate, currently plates can be round or rectangular
 */
enum class PlateType { kROUND, kRECT };

QString toString(PlateType);
PlateType plateTypeFromString(QString const&);

MAKE_MARSHALLABLE(PlateType);
}  // namespace c3picko
