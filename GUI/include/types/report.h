#pragma once

#include "include/colony.hpp"
#include "include/detection_result.h"
#include "include/resource_path.h"
#include "include/types/job.hpp"

class QPdfWriter;
namespace c3picko {
class Database;
class Well {
 public:
  Well(quint8 row, quint8 col, PlateProfile* plate);

  /**
   * @brief Pre increment operator
   * @return Advances the well. Throws on error.
   */
  Well& operator++();
  bool operator<(Well const&) const;

  QString toString() const;

  quint8 row() const;
  quint8 col() const;
  /**
   * @brief Index of the well. Example: A1 = 0, B1 = 1, A2 = 8
   */
  quint16 index() const;

 private:
  /**
   * @brief B1 = (2,1)
   */
  quint8 row_, col_;
  PlateProfile* plate_;
};

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

class ReportCreator {
 public:
  /**
   * @brief Retrives all data from the database and returns a new report filled
   * with it.
   * @param pick_sequence The colonies to be picked in correct order
   */
  static ReportCreator fromDatabase(
      Database& db, Report::ID id, Job::ID job,
      const std::map<Well, Colony::ID>& pick_positions);

  Report createReport() const;
  void writePdfReport(QPdfWriter*) const;

 private:
  ReportCreator(Report::ID id, Job const& job, QDateTime creation,
                const std::map<Well, Colony::ID>& pick_positions,
                const Image& image, const DetectionResult* results_,
                QSet<Colony::ID> colonies_to_pick, Profile const& plate_,
                Profile const& printer_, Profile const& socket_,
                Profile const& octoprint_);
  Report::ID id_;
  Job job_;
  QDateTime creation_;
  /**
   * @brief Maps the colonies to their wells in the order of them being picked.
   */
  std::map<Well, Colony::ID> pick_positions_;
  Image const& image_;
  DetectionResult const* result_;
  QSet<Colony::ID> colonies_to_pick_;
  Profile const& plate_;
  Profile const& printer_;
  Profile const& socket_;
  Profile const& octoprint_;
};
}  // namespace c3picko
