/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include "GUI/types/job.h"
#include "GUI/types/report.h"

namespace d3picko {
/**
 * @brief Holds all information about the exucution of a pick job.
 */
class History {
 private:
  Job::ID job_;
  // Report::ID report_;
};
}  // namespace d3picko
