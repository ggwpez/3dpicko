#include "include/print_job.h"

namespace c3picko {
namespace pi {
PrintJob::PrintJob(GCode gcode) : state(State::NEW), gcode(gcode) {}

void PrintJob::UpdateProgress(data::ProgressInfo new_progress) {
  if (progress != new_progress) {
    progress = new_progress;
    emit OnProgressUpdate(progress);
  }
}
} // namespace pi
} // namespace c3picko
