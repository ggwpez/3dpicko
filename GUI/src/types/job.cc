#include "include/types/job.hpp"

namespace c3picko {
Job::Job(const QJsonObject &obj)
    : JsonConstructable(obj), id_(obj["id"].toString()),
      img_id_(obj["img_id"].toString()), name_(obj["name"].toString()),
      description_(obj["description"].toString()),
      job_created_(parseDateTime(obj["job_created"])),
      printer_(obj["printer"].toString()), socket_(obj["socket"].toString()),
      step_(obj["step"].toInt()) {}

Job::ID Job::id() const { return id_; }

void Job::setId(Job::ID id) { id_ = id; }

void Job::setCreationDate(QDateTime when) { job_created_ = when; }

Image::ID Job::img_id() const { return img_id_; }

void Job::write(QJsonObject &obj) const {
  obj["id"] = id_;
  obj["img_id"] = img_id_;
  obj["name"] = name_;
  obj["description"] = description_;
  obj["job_created"] = job_created_.toMSecsSinceEpoch();
  obj["step"] = step_;
}

Profile::ID Job::socket() const { return socket_; }

Profile::ID Job::printer() const { return printer_; }

Profile::ID Job::plate() const { return plate_; }
} // namespace c3picko
