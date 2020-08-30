/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/responses/job_info.h"

#include <QJsonValue>

namespace c3picko {
namespace pi {
namespace responses {
JobInfo::JobInfo(const QJsonObject& obj) : Response(obj) {
  job = data::JobGeneralInfo(obj["job"].toObject());
  progress = data::ProgressInfo(obj["progress"].toObject());
}
}  // namespace responses
}  // namespace pi
}  // namespace c3picko
