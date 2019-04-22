#include "GUI/types/report.h"
#include "GUI/database.h"
#include "Gcode/plateprofile.h"

namespace c3picko {
Report::Report(Job::ID job, ResourcePath data) : job_(job), data_(data) {}

Job::ID Report::job() const { return job_; }

const ResourcePath& Report::data() const { return data_; }

}  // namespace c3picko
