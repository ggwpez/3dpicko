#pragma once

#include "include/colony.hpp"
#include "include/detection_result.h"
#include "include/resource_path.h"
#include "include/types/job.hpp"

class QPdfWriter;
namespace c3picko {
class Database;
/**
 * @brief Contains information about a planned pick job.
 * Only valid as long, as the database is valid.
 */
class Report {
 public:
  typedef QString ID;

  Report(Job::ID job, ResourcePath pdf, ResourcePath image);

  Job::ID job() const;
  ResourcePath const& pdf() const;
  ResourcePath const& image() const;

 private:
  Job::ID job_;
  ResourcePath pdf_, image_;
};
}  // namespace c3picko
