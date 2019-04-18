#pragma once

#include "include/algorithm.h"
#include "include/colony.h"

namespace c3picko {
class PlateResult;
/**
 * @brief Algorithm for detecting a rectangular Plate on an image under normal
 * light.
 *
 * TODO link
 */
class Plate1 : public Algorithm {
  Q_OBJECT
 public:
  Plate1();

  static void init(AlgorithmJob* base, PlateResult* result);
  static void cvt(AlgorithmJob* base, PlateResult* result);
  static void threshold(AlgorithmJob*, PlateResult* result);
  static void detect(AlgorithmJob* base, PlateResult* result);

  virtual inline Algorithm* cloneEmpty() const override { return new Plate1(); }

 private:
};
}  // namespace c3picko
