#pragma once

#include "include/types/report.h"

namespace c3picko {
class Profile;
class Well;
/**
 * @brief Creates a report with the given data.
 */
class Reporter {
 public:
  /**
   * @brief Retrives all data from the database and returns a new report filled
   * with it.
   * @param pick_sequence The colonies to be picked in correct order
   */
  static Reporter fromDatabase(
      Database& db, Report::ID id, Job::ID job,
      const std::map<Well, Colony::ID>& pick_positions);

  Report createReport() const;

 private:
  Reporter(Report::ID id, Job const& job, QDateTime creation,
           const std::map<Well, Colony::ID>& pick_positions, const Image& image,
           const DetectionResult* results_, QSet<Colony::ID> colonies_to_pick,
           Profile const& plate_, Profile const& printer_,
           Profile const& socket_, Profile const& octoprint_);

  void writePdfReport(QString img_name, QPdfWriter*, QString& html) const;

  QString createProlog() const;
  QString createTitle() const;
  QString createEpilog() const;
  QString createLog() const;
  QString createJobInfo() const;
  QString createImage(QString url) const;
  QString createBr(quint32 width = 1) const;
  /**
   * @param Row wise data
   * @return HTML-table
   */
  QString createTable(QString title, QVector<QString> col_name,
                      QVector<QVector<QString>> data) const;

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
