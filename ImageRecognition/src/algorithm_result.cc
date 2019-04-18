#include "include/algorithm_result.h"
#include <QJsonArray>
#include <QThread>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include "include/colony.h"
#include "include/exception.h"

namespace c3picko {
AlgorithmResult::AlgorithmResult(ID id)
    : id_(id),
      created_(QDateTime::currentDateTime()),
      stages_succeeded_(false),
      cleanup_succeeded_(false),
      is_finalized_(false) {}

AlgorithmResult::~AlgorithmResult() {}

void AlgorithmResult::finalize() {
  if (is_finalized_) throw Exception("Cant finalize twice");
  is_finalized_ = true;
}

AlgorithmResult::ID AlgorithmResult::id() const { return id_; }

QDateTime AlgorithmResult::created() const { return created_; }

QString AlgorithmResult::stageError() const { return stage_error_; }

QString AlgorithmResult::cleanupError() const { return cleanup_error_; }

int AlgorithmResult::lastStage() const { return last_stage_; }

bool AlgorithmResult::cleanupSucceeded() const { return cleanup_succeeded_; }

bool AlgorithmResult::stagesSucceeded() const { return stages_succeeded_; }

template <>
QJsonObject Marshalling::toJson(const AlgorithmResult& value) {
  QJsonObject obj;

  obj["id"] = value.id();
  obj["created"] = Marshalling::toJson(value.created());
  obj["stages_succeeded"] = value.stagesSucceeded();
  obj["cleanup_succeeded"] = value.cleanupSucceeded();
  obj["last_stage"] = value.lastStage();
  obj["stage_error"] = value.stageError();
  obj["cleanup_error"] = value.cleanupError();

  return obj;
}
}  // namespace c3picko
