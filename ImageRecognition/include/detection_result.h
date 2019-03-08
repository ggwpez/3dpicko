#pragma once

#include "include/algorithm_result.h"

namespace c3picko {
class DetectionResult : public AlgorithmResult {
 public:
  using AlgorithmResult::AlgorithmResult;

  struct {
  } plate_;  // TODO
  std::vector<Colony> colonies_;
  std::vector<Colony> colonies() const;
};
MAKE_MARSHALLABLE(DetectionResult);
}  // namespace c3picko
