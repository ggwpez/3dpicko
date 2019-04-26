#pragma once

#include "GUI/types/job.h"
#include "GUI/types/report.h"

namespace c3picko {
/**
 * @brief Holds all information about the exucution of a pick job.
 */
class History {
 private:
  Job::ID job_;
  Report::ID report_;
};
}  // namespace c3picko