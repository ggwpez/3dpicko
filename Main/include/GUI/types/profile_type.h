/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QString>

namespace c3picko {
enum class ProfileType { PRINTER, PLATE, SOCKET, OCTOPRINT };
QString profileToString(ProfileType);
ProfileType profileFromString(QString);
}  // namespace c3picko
