#pragma once

#include "include/colony.hpp"
#include "include/detection_result.h"
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

  /**
   * @brief Retrives all data from the database and returns a new report filled
   * with it.
   * @param pick_sequence The colonies to be picked in correct order
   */
  static Report fromDatabase(Database& db, ID id, Job::ID job,
                             const std::map<Colony::ID, Well>& pick_positions);

  void writePdfReport(QPdfWriter*);

 private:
  Report(ID id, Job const& job, QDateTime creation,
         const std::map<Colony::ID, Well>& pick_positions, const Image& image,
         DetectionResult const& results_, QSet<Colony::ID> colonies_to_pick,
         Profile const& plate_, Profile const& printer_, Profile const& socket_,
         Profile const& octoprint_);
  ID id_;
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
