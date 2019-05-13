#include "ImageRecognition/detection_result.h"
#include <QJsonArray>

namespace c3picko {

template <>
QJsonObject Marshalling::toJson(const DetectionResult& value) {
  QJsonObject obj = Marshalling::toJson<AlgorithmResult>(
	  static_cast<AlgorithmResult const&>(value));
  QJsonArray colonies;

  /*for (Colony const& colony : value.colonies())
								  colonies.push_back(Marshalling::toJson(colony));

  obj["colonies"] = colonies;*/

  return obj;
}

template <>
DetectionResult Marshalling::fromJson(const QJsonObject&) {
  // DetectionResult ret(Marshalling::fromJson<AlgorithmResult>(obj));

  /*for (QJsonValueRef e : obj["colonies"].toArray())
								  ret.colonies().push_back(Marshalling::fromJson<Colony>(e));*/
  throw Exception("Not implemented");
  // return ret;
}

DetectionResult::DetectionResult(AlgorithmResult&& base)
	: MatrixResult(std::move(base)) {}

DetectionResult::~DetectionResult() {}

void DetectionResult::finalize() {
  AlgorithmResult::finalize();

  auto it = std::partition(colonies_.begin(), colonies_.end(),
						   std::bind(&Colony::included, std::placeholders::_1));
  included_end_ = std::distance(colonies_.begin(), it);
}

std::vector<Colony>::iterator DetectionResult::includedBegin() {
  Q_ASSERT(is_finalized_);

  return colonies_.begin();
}

std::vector<Colony>::iterator DetectionResult::includedEnd() {
  Q_ASSERT(is_finalized_);
  auto it = colonies_.begin();
  std::advance(it, included_end_);
  return it;
}

std::vector<Colony>::iterator DetectionResult::excludedBegin() {
  return includedEnd();
}

std::vector<Colony>::iterator DetectionResult::excludedEnd() {
  Q_ASSERT(is_finalized_);
  return colonies_.end();
}

const std::vector<Colony>& DetectionResult::colonies() const {
  return colonies_;
}

std::vector<Colony>& DetectionResult::colonies() { return colonies_; }
}  // namespace c3picko
