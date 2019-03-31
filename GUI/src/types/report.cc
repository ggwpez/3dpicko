#include "include/types/report.h"
#include "include/database.hpp"
#include "include/plateprofile.h"
#include "quazip/JlCompress.h"

namespace c3picko {
Report::Report(Job::ID job, ResourcePath data) : job_(job), data_(data) {}

Job::ID Report::job() const { return job_; }

const ResourcePath& Report::data() const { return data_; }

}  // namespace c3picko
