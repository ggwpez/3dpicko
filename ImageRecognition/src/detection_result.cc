#include "include/detection_result.h"
#include <QJsonArray>

namespace c3picko {

std::vector<Colony> DetectionResult::colonies() const { return colonies_; }

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
}  // namespace c3picko
