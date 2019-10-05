#pragma once

#include <opencv2/core.hpp>
#include "ImageRecognition/colony.h"

#include "ImageRecognition/algorithm.h"
#include "ImageRecognition/detection_result.h"

namespace c3picko {
namespace algorithms {
/**
 * @brief Image detection algorithm for normal light, more description in the
 * wiki.
 */
class Colonies1 : public Algorithm {
  Q_OBJECT
 public:
  Colonies1();
  virtual ~Colonies1() override = default;

  static void cvt(AlgorithmJob* base, DetectionResult* result);
  static void mask(AlgorithmJob* base, DetectionResult* result);
  static void threshold(AlgorithmJob* base, DetectionResult* result);
  static void erodeAndDilate(AlgorithmJob* base, DetectionResult* result);
  static void label(AlgorithmJob* base, DetectionResult* result);
  static void relativeFiltering(AlgorithmJob* base, DetectionResult* result);

  virtual inline Algorithm* cloneEmpty() const override {
	return new Colonies1();
  }

  static void safetyMargin(AlgorithmJob* base, DetectionResult* result);

 private:
};
}  // namespace algorithms
}  // namespace c3picko
