#pragma once

#include "include/algorithm_job.h"
#include "include/detection_result.h"
#include "include/table.hpp"
#include "include/types/image.hpp"
#include "include/types/job.hpp"
#include "include/types/report.h"
#include "include/version.h"

// Cant forward declare bc type traits check in class Table<typename T>
#include "include/types/profile.hpp"

namespace c3picko {
/**
 * @brief A Database. Can be saved and loaded into/from a file.
 * Consists of multiple Tables.
 */
class Database : public QObject, JsonConvertable {
  Q_OBJECT
 public:
  typedef Table<Job> JobTable;
  typedef Table<Image> ImageTable;
  typedef Table<Profile> ProfileTable;
  typedef Table<Version> VersionTable;
  // typedef Table<DetectionResult*> DetectionResultTable;
  // typedef Table<AlgorithmJob*>	AlgoJobTable;

  Database(QSettings& settings, QObject* parent);

  void saveToFile();
  bool readOnly() const;

  JobTable& jobs();
  JobTable& deletedJobs();  // Tracks deleted jobs TODO neeeded?
  ImageTable& images();
  ImageTable& deletedImages();  // "
  ProfileTable& profiles();
  VersionTable& versions();
  // AlgoJobTable&		  algoJobs();
  // DetectionResultTable& detectionResults();

  Job::ID newJobId();
  Profile::ID newProfileId();
  AlgorithmResult::ID newResultId();
  AlgorithmJob::ID newResultJobId();
  Report::ID newReportId();

  Profile::ID defaultPrinter() const;
  Profile::ID defaultSocket() const;
  Profile::ID defaultPlate() const;
  Profile::ID defaultOctoconfig() const;

  void setdefaultPrinter(const Profile::ID& default_printer);
  void setDefaultSocket(const Profile::ID& default_socket);
  void setDefaultPlate(const Profile::ID& default_plate);
  void setDefaultOctoprint(const Profile::ID& default_octo);

  void read(const QJsonObject&) override;
  void write(QJsonObject&) const override;

 private:
  ResourcePath file_path_;
  bool read_only_;

  JobTable jobs_;
  JobTable deleted_jobs_;
  ImageTable images_;
  ImageTable deleted_images_;
  ProfileTable profiles_;
  VersionTable versions_;
  // AlgoJobTable algo_jobs_;
  /**
   * @brief Saves all past image detection processes.
   * TODO create a way to disable it.
   */
  // DetectionResultTable detection_results_;

  qint64 job_id_;
  qint64 profile_id_;
  qint64 result_job_id_;
  qint64 result_id_;
  qint64 report_id_;

  Profile::ID default_printer_;
  Profile::ID default_socket_;
  Profile::ID default_plate_;
  Profile::ID default_octoprint_;
  Version::ID default_version_;
};
}  // namespace c3picko
