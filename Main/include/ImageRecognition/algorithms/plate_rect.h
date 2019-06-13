#pragma once

#include "ImageRecognition/algorithm.h"
#include "ImageRecognition/colony.h"

namespace c3picko {
class PlateResult;
namespace algorithms {
/**
 * @brief Algorithm for detecting a rectangular Plate on an image under normal
 * light.
 *
 * TODO link
 */
class PlateRect : public Algorithm {
  Q_OBJECT
 public:
  PlateRect();

  static void threshold(AlgorithmJob*, PlateResult* result);
  static void detect(AlgorithmJob* base, PlateResult* result);

  virtual inline Algorithm* cloneEmpty() const override {
	return new PlateRect();
  }

 private:
};
}  // namespace algorithms
}  // namespace c3picko
