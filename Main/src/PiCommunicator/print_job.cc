/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/print_job.h"

namespace c3picko {
namespace pi {
PrintJob::PrintJob(GCode gcode) : state(State::NEW), gcode(gcode) {}

void PrintJob::UpdateProgress(data::ProgressInfo new_progress) {
  if (progress != new_progress) {
	progress = new_progress;
	emit OnProgressUpdate(progress);
  }
}
}  // namespace pi
}  // namespace c3picko
