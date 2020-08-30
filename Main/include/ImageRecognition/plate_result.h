/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <memory>

#include "ImageRecognition/matrix_result.h"
#include "ImageRecognition/plates/plate.h"

namespace c3picko {
/**
 * @brief Represents the detected and the corrected plate after a plate
 * detection algorithm
 */
class PlateResult : public MatrixResult {
 public:
  using MatrixResult::MatrixResult;

  const cv::Mat* rotatedImage() const;

  std::unique_ptr<Plate>&& original();
  std::unique_ptr<Plate>&& rotated();

  std::unique_ptr<Plate> const& original() const;
  std::unique_ptr<Plate> const& rotated() const;

 private:
  /**
   * @brief Plate as detected by the algorithm
   */
  std::unique_ptr<Plate> original_;
  /**
   * @brief Plate after rotating it against its rotation angle, such that it is
   * straight horizontal
   */
  std::unique_ptr<Plate> rotated_;
};
MAKE_MARSHALLABLE(PlateResult);
}  // namespace c3picko
