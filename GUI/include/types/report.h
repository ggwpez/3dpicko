#pragma once

#include "include/colony.hpp"
#include "include/detection_result.h"
#include "include/resource_path.h"
#include "include/types/job.hpp"

class QPdfWriter;
namespace c3picko {
class Database;
struct Well {
  /**
   * @brief A1 = (1,1)
   */
  int row, col;

  QString toString() const;
};

/**
 * @brief Contains information about a planned pick job.
 * Only valid as long, as the database is valid.
 */
class Report {
 public:
  typedef QString ID;

  Report(ResourcePath pdf, ResourcePath image);

  ResourcePath const& pdf() const;
  ResourcePath const& image() const;

 private:
  ResourcePath pdf_, image_;
};

class ReportCreator {
 public:
  /**
   * @brief Retrives all data from the database and returns a new report filled
   * with it.
   * @param pick_sequence The colonies to be picked in correct order
   */
  static ReportCreator fromDatabase(
      Database& db, Report::ID id, Job::ID job,
      const std::map<Colony::ID, Well>& pick_positions);

  Report createReport() const;
  void writePdfReport(QPdfWriter*) const;

 private:
  ReportCreator(Report::ID id, Job const& job, QDateTime creation,
                const std::map<Colony::ID, Well>& pick_positions,
                const Image& image, DetectionResult const& results_,
                QSet<Colony::ID> colonies_to_pick, Profile const& plate_,
                Profile const& printer_, Profile const& socket_,
                Profile const& octoprint_);
  Report::ID id_;
  Job job_;
  QDateTime creation_;
  /**
   * @brief Maps the colonies to their wells in the order of them being picked.
   */
  std::map<Colony::ID, Well> pick_positions_;
  Image const& image_;
  DetectionResult const& result_;
  QSet<Colony::ID> colonies_to_pick_;
  Profile const& plate_;
  Profile const& printer_;
  Profile const& socket_;
  Profile const& octoprint_;
};
}  // namespace c3picko
