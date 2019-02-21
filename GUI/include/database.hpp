#pragma once

#include "include/algorithm_result.h"
#include "include/table.hpp"
#include "include/types/image.hpp"
#include "include/types/job.hpp"

// Cant forward declare bc type traits check in class Table<typename T>
#include "include/types/profile.hpp"

namespace c3picko {
class Database : public QObject, JsonConvertable {
  Q_OBJECT
public:
  typedef Table<Job> JobTable;
  typedef Table<Image> ImageTable;
  typedef Table<Profile> ProfileTable;
  typedef Table<AlgorithmResult> DetectionTable;

  Database(QString file_path, QObject *parent);
  ~Database() override;

  void saveToFile();

  JobTable &jobs();
  JobTable &deletedJobs(); // Tracks deleted jobs TODO neeeded?
  ImageTable &images();
  ImageTable &deletedImages(); // "
  ProfileTable &profiles();
  DetectionTable &detections();

  Job::ID newJobId();
  Profile::ID newProfileId();
  AlgorithmResult::ID newResultId();

  Profile::ID defaultPrinter() const;
  Profile::ID defaultSocket() const;
  Profile::ID defaultPlate() const;

  void setdefaultPrinter(const Profile::ID &default_printer);
  void setDefaultSocket(const Profile::ID &default_socket);
  void setDefaultPlate(const Profile::ID &default_plate);

  void read(const QJsonObject &) override;
  void write(QJsonObject &) const override;

private:
  QString file_path_;

  JobTable jobs_;
  JobTable deleted_jobs_;
  ImageTable images_;
  ImageTable deleted_images_;
  ProfileTable profiles_;
  /**
   * @brief Saves all past image detection processes.
   * TODO create a way to disable it.
   */
  DetectionTable detections_;

  qint32 job_id_;
  qint32 profile_id_;
  qint64 result_id_;

  Profile::ID default_printer_;
  Profile::ID default_socket_;
  Profile::ID default_plate_;
};
} // namespace c3picko
