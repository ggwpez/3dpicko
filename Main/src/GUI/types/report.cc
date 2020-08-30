/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "GUI/types/report.h"

#include "GUI/database.h"
#include "Gcode/plateprofile.h"

namespace c3picko {
Report::Report(Job::ID job, ResourcePath data) : job_(job), data_(data) {}

Job::ID Report::job() const { return job_; }

const ResourcePath& Report::data() const { return data_; }

template <>
QJsonObject Marshalling::toJson(Report const& value) {
  return {{"path", value.data().toDocRootAbsolute()}, {"job", value.job()}};
}

template <>
Report Marshalling::fromJson(const QJsonObject& obj) {
  return Report(Marshalling::fromJson<Job::ID>(obj["job"]),
				ResourcePath::fromDocRootAbsolute(obj["path"].toString()));
}

}  // namespace c3picko
