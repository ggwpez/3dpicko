#pragma once

#include "GUI/table.h"
#include "GUI/types/image.h"
#include "GUI/types/job.h"
#include "GUI/types/report.h"
#include "ImageRecognition/algorithm_job.h"
#include "ImageRecognition/detection_result.h"
#include "ImageRecognition/plates/plate.h"
#include "Main/version.h"

// Cant forward declare bc type traits check in class Table<typename T>
#include "GUI/types/profile.h"

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
  typedef QVector<Version::ID> VersionIdVector;
  typedef Table<std::unique_ptr<Plate>> PlateTable;

  Database(QSettings const& settings, QObject* parent);
  ~Database() override;

  bool readOnly() const;
  bool checkIntegrity() const;

  JobTable& jobs();
  JobTable const& jobs() const;
  JobTable& deletedJobs();  // Tracks deleted jobs TODO neeeded?
  JobTable const& deletedJobs() const;
  ImageTable& images();
  ImageTable const& images() const;
  ImageTable& deletedImages();  // "
  ImageTable const& deletedImages() const;
  ProfileTable& profiles();
  ProfileTable const& profiles() const;
  VersionTable& versions();
  VersionTable const& versions() const;
  VersionIdVector& installedVersions();
  VersionIdVector const& installedVersions() const;
  PlateTable& detectedPlates();
  PlateTable const& detectedPlates() const;

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

 signals:
  /**
   * @brief Emitted when any data content was changed .
   */
  void OnDataChanged();

 public slots:
  void saveToFile();
  void autosave();
  void autosaveSkipped();

 private:
  ResourcePath file_path_;
  bool read_only_;

  JobTable jobs_;
  JobTable deleted_jobs_;
  ImageTable images_;
  ImageTable deleted_images_;
  ProfileTable profiles_;
  VersionTable versions_;
  PlateTable detected_plates_;
  /**
   * @brief All installed versions.
   * Ordered by date; front is old and back is new.
   */
  VersionIdVector versions_installed_;
  // AlgoJobTable algo_jobs_;
  /**
   * @brief Saves all past image detection processes.
   * TODO create a way to disable it.
   */
  // DetectionResultTable detection_results_;

  /**
   * @brief New unused job id.
   */
  qint64 job_id_;
  /**
   * @brief New unused profile id.
   */
  qint64 profile_id_;
  /**
   * @brief New unused result job id.
   */
  qint64 result_job_id_;
  /**
   * @brief New unused result id.
   */
  qint64 result_id_;
  /**
   * @brief New unused report id.
   */
  qint64 report_id_;

  Profile::ID default_printer_;
  Profile::ID default_socket_;
  Profile::ID default_plate_;
  Profile::ID default_octoprint_;
  Version::ID default_version_;
};
}  // namespace c3picko
