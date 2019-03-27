#include "include/plate_result.h"
#include <opencv2/imgproc.hpp>
#include <tuple>

namespace c3picko {
std::shared_ptr<cv::Mat> PlateResult::rotatedImage() const {
  if (stack_.empty() || !stack_.back()) return nullptr;

  return stack_.back();
}
}  // namespace c3picko
