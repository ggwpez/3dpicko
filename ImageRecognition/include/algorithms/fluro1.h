#pragma once

#include "include/algorithm.h"

namespace c3picko {
class Fluro1 : public Algorithm {
  Q_OBJECT
 public:
  Fluro1();

  static void threshold(AlgorithmJob* base, AlgorithmResult* result);

  virtual inline Algorithm* cloneEmpty() const override { return new Fluro1(); }
};
}  // namespace c3picko
