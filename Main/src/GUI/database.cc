#include "GUI/database.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>

namespace c3picko {
Database::Database(const QSettings& settings, QObject* parent)
	: QObject(parent),
	  file_path_(paths::root() + "/database.json"),
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
	else
	  read(json);
  } else if (ignore_empty)
	qDebug() << "Database not found, creating a new one (ignoreEmpty=true)";
  else
	throw Exception("Error reading database from file: " +
					file_path_.toSystemAbsolute() + " (ignoreEmpty=false)");

  if (settings.value("check", false).toBool()) checkIntegrity();
}

Database::~Database() { saveToFile(); }

void Database::saveToFile() {
  try {
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
  } catch (...) {
	qCritical() << "Error saving database to file:"
				<< file_path_.toSystemAbsolute();
  }
}

void Database::autosave() {
  qDebug() << "Autosave";
  saveToFile();
}

void Database::autosaveSkipped() {
  qDebug() << "Skipped Autosave (no changes)";
}

Database::JobTable& Database::jobs() {
  emit OnDataChanged();
  return jobs_;
}

const Database::JobTable& Database::jobs() const { return jobs_; }

Database::ImageTable& Database::images() {
  emit OnDataChanged();
  return images_;
}

const Database::ImageTable& Database::images() const { return images_; }

Database::ProfileTable& Database::profiles() {
  emit OnDataChanged();
  return profiles_;
}

const Database::ProfileTable& Database::profiles() const { return profiles_; }

Database::VersionTable& Database::versions() {
  emit OnDataChanged();
  return versions_;
}

const Database::VersionTable& Database::versions() const { return versions_; }

Database::VersionIdVector& Database::installedVersions() {
  emit OnDataChanged();
  return versions_installed_;
}

const Database::VersionIdVector& Database::installedVersions() const {
  return versions_installed_;
}

Database::PlateTable& Database::detectedPlates() { return detected_plates_; }

const Database::PlateTable& Database::detectedPlates() const {
  return detected_plates_;
}

Job::ID Database::newJobId() {
  emit OnDataChanged();
  return QString::number(job_id_++);
}  // i guess it is ok the emmit first...

Profile::ID Database::newProfileId() {
  emit OnDataChanged();
  return QString::number(profile_id_++);
}

AlgorithmResult::ID Database::newResultId() {
  emit OnDataChanged();
  return QString::number(result_id_++);
}

AlgorithmJob::ID Database::newResultJobId() {
  emit OnDataChanged();
  return QString::number(result_job_id_++);
}

Report::ID Database::newReportId() {
  emit OnDataChanged();
  return QString::number(report_id_++);
}

Profile::ID Database::defaultPrinter() const { return default_printer_; }

Profile::ID Database::defaultSocket() const { return default_socket_; }

Profile::ID Database::defaultPlate() const { return default_plate_; }

Profile::ID Database::defaultOctoconfig() const { return default_octoprint_; }

void Database::read(QJsonObject const& obj) {
  jobs_.read(obj["jobs"].toObject());
  images_.read(obj["images"].toObject());
  profiles_.read(obj["profiles"].toObject());
  versions_.read(obj["versions"].toObject());
  detected_plates_.read(obj["detected_plates"].toObject());

  QJsonArray const& arr(obj["versions_of_interest"].toArray());
  for (auto e : arr)  // TODO
	versions_installed_.push_back(e.toString());
  // versions_oi_.read(obj["versions_of_interest"].toObject());

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
  obj["images"] = (QJsonObject)images_;
  obj["profiles"] = (QJsonObject)profiles_;
  obj["versions"] = (QJsonObject)versions_;
  obj["detected_plates"] = (QJsonObject)detected_plates_;

  QJsonArray arr;
  for (auto e : versions_installed_) arr.push_back(e);
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

bool Database::checkIntegrity() const {
  QStringList errors;

  for (auto const& pair : jobs_) {
	Job const& job = pair.second;
	if (!profiles_.exists(job.plate()) || !profiles_.exists(job.printer()) ||
		!profiles_.exists(job.socket()) || !profiles_.exists(job.octoprint()))
	  errors << ("Lost Profile of job " + job.id());
	if (!images_.exists(job.imgID()))
	  errors << ("Lost Image   of job " + job.id());
  }

  if (!errors.empty()) {
	qWarning().nospace() << "Database errors (" << errors.size() << "):";
	for (QString error : errors) qWarning().noquote() << "" << error;
  }

  return errors.empty();
}

void Database::setdefaultPrinter(const Profile::ID& default_printer) {
  default_printer_ = default_printer;
  emit OnDataChanged();
}

void Database::setDefaultSocket(const Profile::ID& default_socket) {
  default_socket_ = default_socket;
  emit OnDataChanged();
}

void Database::setDefaultPlate(const Profile::ID& default_plate) {
  default_plate_ = default_plate;
  emit OnDataChanged();
}

void Database::setDefaultOctoprint(const Profile::ID& default_octo) {
  default_octoprint_ = default_octo;
  emit OnDataChanged();
}
}  // namespace c3picko
