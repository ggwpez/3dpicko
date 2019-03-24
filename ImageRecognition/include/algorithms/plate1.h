#pragma once

#include "include/algorithm.h"
#include "include/colony.hpp"

namespace c3picko {
class PlateResult;
class Plate1 : public Algorithm {
  Q_OBJECT
 public:
  Plate1();

  static void cvt(AlgorithmJob *base, PlateResult *result);
  static void threshold(AlgorithmJob *, PlateResult *result);
  static void detect(AlgorithmJob *base, PlateResult *result);

  virtual inline Algorithm *cloneEmpty() const override { return new Plate1(); }

 private:
};
}  // namespace c3picko
