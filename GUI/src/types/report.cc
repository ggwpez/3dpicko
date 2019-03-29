#include "include/types/report.h"
#include "include/database.hpp"
#include "include/plateprofile.h"
#include "quazip/JlCompress.h"

namespace c3picko {
Report::Report(Job::ID job, ResourcePath pdf, ResourcePath image)
    : job_(job), pdf_(pdf), image_(image) {}

static void test() {
  // Link Test
  JlCompress::compressDir("");
}
Job::ID Report::job() const { return job_; }
ResourcePath const& Report::pdf() const { return pdf_; }

const ResourcePath& Report::image() const { return image_; }

}  // namespace c3picko
