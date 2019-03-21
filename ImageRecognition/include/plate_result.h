#pragma once

#include "include/algorithm_result.h"
#include "include/plate.h"

namespace c3picko {
class PlateResult : public AlgorithmResult {
public:
  using AlgorithmResult::AlgorithmResult;

  Plate original_, rotated_;
};
MAKE_MARSHALLABLE(PlateResult);
} // namespace c3picko
