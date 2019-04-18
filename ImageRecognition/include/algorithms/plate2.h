#pragma once

#include "include/algorithm.h"
#include "include/colony.h"

namespace c3picko {
class PlateResult;
/**
 * @brief Algorithm for detecting a round Plate on an image under normal
 * light.
 *
 * TODO link
 */
class Plate2 : public Algorithm {
  Q_OBJECT
 public:
  Plate2();

  static void detect(AlgorithmJob* base, PlateResult* result);

  virtual inline Algorithm* cloneEmpty() const override { return new Plate2(); }
};
}  // namespace c3picko
