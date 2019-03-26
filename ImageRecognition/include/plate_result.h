#pragma once

#include "include/matrix_result.h"
#include "include/plate.h"

namespace c3picko {
/**
 * @brief Represents the detected and the corrected plate after a plate
 * detection algorithm
 */
class PlateResult : public MatrixResult {
 public:
  using MatrixResult::MatrixResult;

  /**
   * @brief Plate as detected by the algorithm
   */
  Plate original_;
  /**
   * @brief Plate after rotating it against its rotation angle, such that it is
   * straight horizontal
   */
  Plate rotated_;

  cv::Mat* rotatedImage() const;
};
MAKE_MARSHALLABLE(PlateResult);
}  // namespace c3picko
