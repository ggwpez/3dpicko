#pragma once

#include "ImageRecognition/algorithm.h"
#include "ImageRecognition/colony.h"

namespace c3picko {
class PlateResult;
namespace algorithms {
/**
 * @brief Algorithm for detecting a round Plate on an image under normal
 * light.
 *
 * TODO link
 */
class PlateRound : public Algorithm {
  Q_OBJECT
 public:
  PlateRound();

  static void detect(AlgorithmJob* base, PlateResult* result);

  virtual inline Algorithm* cloneEmpty() const override {
	return new PlateRound();
  }
};
}  // namespace algorithms
}  // namespace c3picko
