/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/commands/job_operation.h"

namespace d3picko {
namespace pi {
namespace commands {

JobOperation* JobOperation::GetInfo() {
  return new JobOperation("job", {200}, HTTPType::GET);
}

JobOperation* JobOperation::Start() {
  return new JobOperation("job", {{"command", "start"}}, {204}, HTTPType::POST);
}

JobOperation* JobOperation::Cancel() {
  return new JobOperation("job", {{"command", "cancel"}}, {204},
						  HTTPType::POST);
}

JobOperation* JobOperation::Restart() {
  return new JobOperation("job", {{"command", "restart"}}, {204},
						  HTTPType::POST);
}

JobOperation* JobOperation::Pause() {
  return new JobOperation("job", {{"command", "pause"}, {"action", "pause"}},
						  {204}, HTTPType::POST);
}

JobOperation* JobOperation::Resume() {
  return new JobOperation("job", {{"command", "pause"}, {"action", "resume"}},
						  {204}, HTTPType::POST);
}

JobOperation* JobOperation::TogglePause() {
  return new JobOperation("job", {{"command", "pause"}, {"action", "toggle"}},
						  {204}, HTTPType::POST);
}

void JobOperation::OnReplyFinished(QNetworkReply* reply) {
  if (type_ == HTTPType::GET)
	CheckStatusCodeAndResponse<Response>(reply);
  else
	CheckStatusCode(reply);
}
}  // namespace commands
}  // namespace pi
}  // namespace d3picko
