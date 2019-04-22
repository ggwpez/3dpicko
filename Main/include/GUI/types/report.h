#pragma once

#include "ImageRecognition/colony.h"
#include "ImageRecognition/detection_result.h"
#include "Main/resource_path.h"
#include "GUI/types/job.h"

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

  Report(Job::ID job, ResourcePath data);

  Job::ID job() const;
  ResourcePath const& data() const;

 private:
  Job::ID job_;
  /**
   * @brief Path to the downloadable report.
   */
  ResourcePath data_;
};
}  // namespace c3picko
