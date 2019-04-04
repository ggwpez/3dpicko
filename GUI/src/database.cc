#include "include/database.hpp"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>

namespace c3picko {
Database::Database(QSettings& settings, QObject* parent)
    : QObject(parent),
      file_path_(Root() + "/database.json"),
      read_only_(!settings.value("saveChanges", true).toBool()),
      job_id_(200),
      profile_id_(300) {
  const bool ignore_empty = settings.value("ignoreEmpty", true).toBool();
  QFile file(file_path_.toSystemAbsolute());

  if (file.open(QIODevice::ReadOnly)) {
    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonObject json = QJsonDocument::fromJson(data, &error).object();

    if (error.error)
      throw Exception("Cannot parse JSON database: " + error.errorString());
    else {
      read(json);
      qDebug().nospace() << "Loaded database from file: "
                         << file_path_.toSystemAbsolute()
                         << " (saveChanges=" << !read_only_ << ")";
    }
  } else if (ignore_empty)
    qDebug() << "Database not found, creating new one (ignoreEmpty=true)";
  else
    throw Exception("Error reading database from file: " +
                    file_path_.toSystemAbsolute() + " (ignoreEmpty=false)");
}

void Database::saveToFile() {
  if (read_only_) {
    qDebug() << "NOT saving database changes (saveChanges=false)";
    return;
  }

  QFile file(file_path_.toSystemAbsolute());

  if (file.open(QIODevice::WriteOnly)) {
    QJsonObject json;
    write(json);
    QByteArray data = QJsonDocument(json).toJson();

    file.write(data);
    qDebug() << "Saving database to file:" << file_path_.toSystemAbsolute();
  } else
    qCritical() << "Error saving database to file:"
                << file_path_.toSystemAbsolute();
}

Database::JobTable& Database::jobs() { return jobs_; }

Database::ImageTable& Database::images() { return images_; }

Database::ImageTable& Database::deletedImages() { return deleted_images_; }

Database::ProfileTable& Database::profiles() { return profiles_; }

Database::VersionTable& Database::versions() { return versions_; }

Database::VersionIdVector& Database::installedVersions() {
  return versions_oi_;
}

// Database::AlgoJobTable& Database::algoJobs() { return algo_jobs_; }

// Database::DetectionResultTable& Database::detectionResults() { return
// detection_results_; }

Job::ID Database::newJobId() { return QString::number(job_id_++); }

Profile::ID Database::newProfileId() { return QString::number(profile_id_++); }

AlgorithmResult::ID Database::newResultId() {
  return QString::number(result_id_++);
}

AlgorithmJob::ID Database::newResultJobId() {
  return QString::number(result_job_id_++);
}

Report::ID Database::newReportId() { return QString::number(report_id_++); }

Profile::ID Database::defaultPrinter() const { return default_printer_; }

Profile::ID Database::defaultSocket() const { return default_socket_; }

Profile::ID Database::defaultPlate() const { return default_plate_; }

Profile::ID Database::defaultOctoconfig() const { return default_octoprint_; }

Database::JobTable& Database::deletedJobs() { return deleted_jobs_; }

void Database::read(QJsonObject const& obj) {
  jobs_.read(obj["jobs"].toObject());
  deleted_jobs_.read(obj["deleted_jobs"].toObject());
  images_.read(obj["images"].toObject());
  deleted_images_.read(obj["deleted_images"].toObject());
  profiles_.read(obj["profiles"].toObject());
  versions_.read(obj["versions"].toObject());

  QJsonArray const& arr(obj["versions_of_interest"].toArray());
  for (auto e : arr)  // TODO
    versions_oi_.push_back(e.toString());
  // versions_oi_.read(obj["versions_of_interest"].toObject());

  // TODO read detection*

  job_id_ = obj["job_id"].toInt();
  profile_id_ = obj["profile_id"].toInt();
  result_id_ = obj["result_id"].toInt();
  result_job_id_ = obj["result_job_id"].toInt();
  report_id_ = obj["report_id"].toInt();

  default_printer_ = Marshalling::fromJson<Profile::ID>(obj["default_printer"]);
  default_socket_ = Marshalling::fromJson<Profile::ID>(obj["default_socket"]);
  default_plate_ = Marshalling::fromJson<Profile::ID>(obj["default_plate"]);
  default_octoprint_ =
      Marshalling::fromJson<Profile::ID>(obj["default_octoprint"]);
  default_version_ = Marshalling::fromJson<Version::ID>(obj["default_version"]);
}

void Database::write(QJsonObject& obj) const {
  obj["jobs"] = (QJsonObject)jobs_;
  obj["deleted_jobs"] = (QJsonObject)deleted_jobs_;
  obj["images"] = (QJsonObject)images_;
  obj["deleted_images"] = (QJsonObject)deleted_images_;
  obj["profiles"] = (QJsonObject)profiles_;
  obj["versions"] = (QJsonObject)versions_;

  QJsonArray arr;
  for (auto e : versions_oi_) arr.push_back(e);
  obj["versions_of_interest"] = arr;
  // obj["versions_of_interest"] = (QJsonObject)versions_oi_;

  // obj["detection_jobs"] = (QJsonObject)detection_jobs_;
  // obj["detection_results"] = (QJsonObject)detection_results_;

  obj["job_id"] = job_id_;
  obj["profile_id"] = profile_id_;
  obj["result_id"] = result_id_;
  obj["result_job_id"] = result_job_id_;
  obj["report_id"] = report_id_;

  obj["default_printer"] = default_printer_;
  obj["default_socket"] = default_socket_;
  obj["default_plate"] = default_plate_;
  obj["default_octoprint"] = default_octoprint_;
  obj["default_version"] = default_version_;
}

bool Database::readOnly() const { return read_only_; }

void Database::setdefaultPrinter(const Profile::ID& default_printer) {
  default_printer_ = default_printer;
}

void Database::setDefaultSocket(const Profile::ID& default_socket) {
  default_socket_ = default_socket;
}

void Database::setDefaultPlate(const Profile::ID& default_plate) {
  default_plate_ = default_plate;
}

void Database::setDefaultOctoprint(const Profile::ID& default_octo) {
  default_octoprint_ = default_octo;
}
}  // namespace c3picko
