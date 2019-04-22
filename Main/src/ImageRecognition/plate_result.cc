#include "ImageRecognition/plate_result.h"
#include <opencv2/imgproc.hpp>
#include <tuple>

namespace c3picko {
cv::Mat const* PlateResult::rotatedImage() const {
  if (stack_.empty()) return nullptr;

  return &stack_.back();
}
}  // namespace c3picko
