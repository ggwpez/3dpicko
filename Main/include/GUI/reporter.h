#pragma once

#include "GUI/types/report.h"

namespace c3picko {
class GcodeInstruction;
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
  static Reporter fromDatabase(Database const& db, Job::ID job,
							   const std::map<Well, Colony::ID>& pick_positions,
							   const std::vector<GcodeInstruction>& gcode);

  /**
   * @brief Creates a report with the data read from the fromData call.
   * Saves the output to reports/ .
   */
  Report createReport() const;

 private:
  QString createImage(QString url) const;
  Reporter(Job const& job, QDateTime creation,
		   const std::map<Well, Colony::ID>& pick_positions, const Image& image,
		   const DetectionResult* results_, QSet<Colony::ID> colonies_to_pick,
		   Profile const& plate, Profile const& printer, Profile const& socket,
		   Profile const& octoprint,
		   const std::vector<GcodeInstruction>& gcode);

  void writeHtmlReport(cv::Mat const& img_data, QString& html) const;

  /**
   * @brief Helper function for HTML creation.
   */
  QString createProlog() const;
  /**
   * @brief Helper function for HTML creation.
   */
  QString createTitle() const;
  /**
   * @brief Helper function for HTML creation.
   */
  QString createEpilog() const;
  /**
   * @brief Helper function for HTML creation.
   */
  QString createLog() const;
  /**
   * @brief Helper function for HTML creation.
   */
  QString createJobInfo() const;
  /**
   * @brief Helper function for HTML creation.
   */
  QString createImage(QByteArray base64) const;
  /**
   * @brief Helper function for HTML creation.
   */
  QString createBr(quint32 width = 1) const;
  /**
   * @brief Helper function for HTML creation.
   * @param data is row wise.
   */
  QString createTable(QString title, QVector<QString> col_name,
					  QVector<QVector<QString>> data) const;

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
  std::vector<GcodeInstruction> gcode_;
};
}  // namespace c3picko
