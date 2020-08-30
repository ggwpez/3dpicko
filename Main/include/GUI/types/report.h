/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include "GUI/types/job.h"
#include "ImageRecognition/colony.h"
#include "ImageRecognition/detection_result.h"
#include "Main/resource_path.h"

class QPdfWriter;
namespace c3picko {
class Database;
/**
 * @brief Contains information about a planned pick job.
 * Only valid as long, as the database is valid.
 */
class Report {
 public:
  Report(Job::ID job, ResourcePath data);

  Job::ID job() const;
  ResourcePath const& data() const;

 private:
  Job::ID job_;
  /**
   * @brief Path to the downloadable report.
   */
  ResourcePath data_;
};
MAKE_MARSHALLABLE(Report)
}  // namespace c3picko
