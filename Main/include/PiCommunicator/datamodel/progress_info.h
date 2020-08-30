/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef PROGRESS_INFO_H
#define PROGRESS_INFO_H

#include <QtGlobal>
class QJsonObject;

namespace c3picko {
namespace pi {
namespace data {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#progress-information)
 */
struct ProgressInfo {
  ProgressInfo() = default;
  ProgressInfo(QJsonObject const&);
  bool operator!=(ProgressInfo const& other);

  /**
   * @brief Percentage of completion of the current print job
   */
  qreal completion;
  /**
   * @brief Current position in the file being printed, in bytes from the
   * beginning
   */
  qint32 filepos;
  /**
   * @brief Time already spent printing, in seconds
   */
  qint32 print_time;
  /**
   * @brief Estimate of time left to print, in seconds
   */
  qint32 print_time_left;
};
}  // namespace data
}  // namespace pi
}  // namespace c3picko

#endif	// PROGRESS_INFO_H
