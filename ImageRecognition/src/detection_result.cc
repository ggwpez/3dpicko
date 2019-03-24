#include "include/detection_result.h"
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
DetectionResult Marshalling::fromJson(const QJsonObject& obj) {
  throw Exception("Not implemented");
}

void DetectionResult::finalize() {
  AlgorithmResult::finalize();

  included_end_ =
      std::partition(colonies_.begin(), colonies_.end(),
                     std::bind(&Colony::included, std::placeholders::_1));
}

std::vector<Colony>::iterator DetectionResult::includedBegin() {
  return colonies_.begin();
}

std::vector<Colony>::iterator DetectionResult::includedEnd() {
  return included_end_;
}

std::vector<Colony>::iterator DetectionResult::excludedBegin() {
  return included_end_;
}

std::vector<Colony>::iterator DetectionResult::excludedEnd() {
  return colonies_.end();
}

std::vector<Colony>& DetectionResult::colonies() { return colonies_; }

}  // namespace c3picko
