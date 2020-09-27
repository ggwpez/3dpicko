/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef JOBGENERALINFO_H
#define JOBGENERALINFO_H

#include "PiCommunicator/datamodel/abridged_file_info.h"

class QJsonObject;

namespace d3picko {
namespace pi {
namespace data {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#job-information)
 */
struct JobGeneralInfo {
  JobGeneralInfo() = default;
  JobGeneralInfo(QJsonObject const&);

  AbridgedFileInfo file;
  qint32 estimated_print_time;
  qint32 last_print_time;

  struct {
	qint32 length;
	qreal volume;
  } filament;
};
}  // namespace data
}  // namespace pi
}  // namespace d3picko
#endif	// JOBGENERALINFO_H
