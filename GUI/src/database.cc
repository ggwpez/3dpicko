#include "include/database.hpp"
#include <QDebug>
#include <QJsonDocument>

namespace c3picko {
Database::Database(QString file_path, QObject* parent)
    : QObject(parent), file_path_(file_path), job_id_(200), profile_id_(300) {
  QFile file(file_path);

  if (file.open(QIODevice::ReadOnly)) {
    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonObject json = QJsonDocument::fromJson(data, &error).object();

    if (error.error) {
      qFatal("Cannot parse JSON database: %s", qPrintable(error.errorString()));
    } else {
      read(json);
      qDebug() << "Loaded database from file" << file_path_;
    }
  } else
    qCritical() << "Error reading database from file" << file_path_;
}

Database::~Database() { saveToFile(); }

void Database::saveToFile() {
  QFile file(file_path_);

  if (file.open(QIODevice::WriteOnly)) {
    QJsonObject json;
    write(json);
    QByteArray data = QJsonDocument(json).toJson();

    file.write(data);
    qDebug() << "Saving database to file" << file_path_;
  } else
    qCritical() << "Error saving database to file" << file_path_;
}

Database::JobTable& Database::jobs() { return jobs_; }

Database::ImageTable& Database::images() { return images_; }

Database::ImageTable& Database::deletedImages() { return deleted_images_; }

Database::ProfileTable& Database::profiles() { return profiles_; }

// Database::DetectionJobTable& Database::detectionJobs() { return
// detection_jobs_; }

Database::DetectionResultTable& Database::detectionResults() {
  return detection_results_;
}

Job::ID Database::newJobId() { return QString::number(job_id_++); }

Profile::ID Database::newProfileId() { return QString::number(profile_id_++); }

AlgorithmResult::ID Database::newResultId() {
  return QString::number(result_id_++);
}

AlgorithmJob::ID Database::newResultJobId() {
  return QString::number(result_job_id_++);
}

Profile::ID Database::defaultPrinter() const { return default_printer_; }

Profile::ID Database::defaultSocket() const { return default_socket_; }

Profile::ID Database::defaultPlate() const { return default_plate_; }

Database::JobTable& Database::deletedJobs() { return deleted_jobs_; }

void Database::read(QJsonObject const& obj) {
  jobs_.read(obj["jobs"].toObject());
  deleted_jobs_.read(obj["deleted_jobs"].toObject());
  images_.read(obj["images"].toObject());
  deleted_images_.read(obj["deleted_images"].toObject());
  profiles_.read(obj["profiles"].toObject());

  // TODO read detection*

  job_id_ = obj["job_id"].toInt();
  profile_id_ = obj["profile_id"].toInt();
  result_id_ = obj["result_id"].toInt();
  result_job_id_ = obj["result_job_id"].toInt();

  default_printer_ = Marshalling::fromJson<QString>(obj["default_printer"]);
  default_socket_ = Marshalling::fromJson<QString>(obj["default_socket"]);
  default_plate_ = Marshalling::fromJson<QString>(obj["default_plate"]);
}

void Database::write(QJsonObject& obj) const {
  obj["jobs"] = (QJsonObject)jobs_;
  obj["deleted_jobs"] = (QJsonObject)deleted_jobs_;
  obj["images"] = (QJsonObject)images_;
  obj["deleted_images"] = (QJsonObject)deleted_images_;
  obj["profiles"] = (QJsonObject)profiles_;

  // obj["detection_jobs"] = (QJsonObject)detection_jobs_;
  obj["detection_results"] = (QJsonObject)detection_results_;

  obj["job_id"] = job_id_;
  obj["profile_id"] = profile_id_;
  obj["result_id"] = result_id_;
  obj["result_job_id"] = result_job_id_;

  obj["default_printer"] = default_printer_;
  obj["default_socket"] = default_socket_;
  obj["default_plate"] = default_plate_;
}

void Database::setdefaultPrinter(const Profile::ID& default_printer) {
  default_printer_ = default_printer;
}

void Database::setDefaultSocket(const Profile::ID& default_socket) {
  default_socket_ = default_socket;
}

void Database::setDefaultPlate(const Profile::ID& default_plate) {
  default_plate_ = default_plate;
}
}  // namespace c3picko
