#include "include/types/job.hpp"

namespace c3picko {
Job::Job(ID id, Image::ID img_id, QString name, QString description,
         QDateTime job_created, Profile::ID printer, Profile::ID socket,
         int step)
    : id_(id), img_id_(img_id), name_(name), description_(description),
      job_created_(job_created), printer_(printer), socket_(socket),
      step_(step) {}

Job::ID Job::id() const { return id_; }

void Job::setId(Job::ID id) { id_ = id; }

void Job::setCreationDate(QDateTime when) { job_created_ = when; }

Image::ID Job::imgID() const { return img_id_; }

Profile::ID Job::socket() const { return socket_; }

Profile::ID Job::printer() const { return printer_; }

Profile::ID Job::plate() const { return plate_; }

AlgorithmResult::ID Job::resultID() const { return result_id_; }

void Job::setResultID(const AlgorithmResult::ID &result_id) {
  result_id_ = result_id;
}

qint32 Job::step() const { return step_; }

QDateTime Job::job_created() const { return job_created_; }

QString Job::name() const { return name_; }

QString Job::description() const { return description_; }

template <> QJsonObject Marshalling::toJson(const Job &value) {
  QJsonObject obj;

  obj["id"] = value.id();
  obj["img_id"] = value.imgID();
  obj["name"] = value.name();
  obj["description"] = value.description();
  obj["job_created"] = value.job_created().toMSecsSinceEpoch();
  obj["printer"] = value.printer();
  obj["socket"] = value.socket();
  obj["step"] = value.step();

  return obj;
}

template <> Job Marshalling::fromJson(const QJsonObject &obj) {
  return Job(obj["id"].toString(), obj["img_id"].toString(),
             obj["name"].toString(), obj["description"].toString(),
             parseDateTime(obj["job_created"]), obj["printer"].toString(),
             obj["socket"].toString(), obj["step"].toInt());
}
} // namespace c3picko
