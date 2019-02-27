#include "include/types/job.hpp"
#include <QJsonArray>

namespace c3picko {
Job::Job(ID id, Image::ID img_id, QString name, QString description,
         QDateTime job_created, QStack<AlgorithmResult::ID> results,
         Profile::ID printer, Profile::ID socket, int starting_row,
         int starting_col, int step)
    : id_(id),
      img_id_(img_id),
      name_(name),
      description_(description),
      job_created_(job_created),
      result_ids_(results),
      printer_(printer),
      socket_(socket),
      starting_row_(starting_row),
      starting_col_(starting_col),
      step_(step) {}

Job::ID Job::id() const { return id_; }

void Job::setId(Job::ID id) { id_ = id; }

void Job::setCreationDate(QDateTime when) { job_created_ = when; }

Image::ID Job::imgID() const { return img_id_; }

Profile::ID Job::socket() const { return socket_; }

Profile::ID Job::printer() const { return printer_; }

Profile::ID Job::plate() const { return plate_; }

AlgorithmResult::ID Job::resultID() const {
  if (result_ids_.empty())
    return AlgorithmResult::ID();
  else
    return result_ids_.top();
}

QStack<AlgorithmResult::ID> Job::resultIDs() const { return result_ids_; }

void Job::setResultID(const AlgorithmResult::ID &result_id) {
  result_ids_.push(result_id);
}

int Job::startingCol() const { return starting_col_; }

void Job::setStartingCol(int starting_col) { starting_col_ = starting_col; }

int Job::startingRow() const { return starting_row_; }

void Job::setStartingRow(int starting_row) { starting_row_ = starting_row; }

void Job::setPlate(const Profile::ID &plate) { plate_ = plate; }

qint32 Job::step() const { return step_; }

QDateTime Job::job_created() const { return job_created_; }

QString Job::name() const { return name_; }

QString Job::description() const { return description_; }

template <>
QJsonObject Marshalling::toJson(const Job &value) {
  QJsonObject obj;

  obj["id"] = value.id();
  obj["img_id"] = value.imgID();
  obj["name"] = value.name();
  obj["description"] = value.description();
  obj["job_created"] = value.job_created().toMSecsSinceEpoch();
  obj["printer"] = value.printer();
  obj["socket"] = value.socket();
  obj["step"] = value.step();
  obj["starting_row"] = value.startingRow();
  obj["starting_col"] = value.startingCol();

  QJsonArray detections;
  for (auto const &detection : value.resultIDs())
    detections.push_back(detection);

  obj["detections"] = detections;

  return obj;
}

template <>
Job Marshalling::fromJson(const QJsonObject &obj) {
  QJsonArray json_detections = obj["detections"].toArray();
  QStack<AlgorithmResult::ID> detections;

  for (auto detection : json_detections) detections.push(detection.toString());

  return Job(obj["id"].toString(), obj["img_id"].toString(),
             obj["name"].toString(), obj["description"].toString(),
             parseDateTime(obj["job_created"]), detections,
             obj["printer"].toString(), obj["socket"].toString(),
             obj["starting_row"].toInt(), obj["starting_col"].toInt(),
             obj["step"].toInt());
}
}  // namespace c3picko
