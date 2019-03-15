#pragma once

#include "include/algorithm_result.h"

namespace c3picko {
class DetectionResult : public AlgorithmResult {
 public:
  using AlgorithmResult::AlgorithmResult;

  std::vector<Colony>& colonies();

 private:
  std::vector<Colony> colonies_;
};
MAKE_MARSHALLABLE(DetectionResult);
}  // namespace c3picko
