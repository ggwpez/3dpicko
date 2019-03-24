#pragma once

#include "include/algorithm_result.h"

namespace c3picko {
class DetectionResult : public AlgorithmResult {
 public:
  using AlgorithmResult::AlgorithmResult;
  virtual ~DetectionResult() = default;

  virtual void finalize() override;

  std::vector<Colony>::iterator includedBegin();
  std::vector<Colony>::iterator includedEnd();

  std::vector<Colony>::iterator excludedBegin();
  std::vector<Colony>::iterator excludedEnd();

  std::vector<Colony>& colonies();

 private:
  std::vector<Colony> colonies_;
  std::vector<Colony>::iterator included_end_;
};
MAKE_MARSHALLABLE(DetectionResult);
}  // namespace c3picko
