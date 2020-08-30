/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "ImageRecognition/plate_result.h"

#include <opencv2/imgproc.hpp>
#include <tuple>

namespace c3picko {
cv::Mat const* PlateResult::rotatedImage() const {
  if (stack_.empty()) return nullptr;

  return &stack_.back();
}

std::unique_ptr<Plate>&& PlateResult::original() {
  return std::move(original_);
}

std::unique_ptr<Plate>&& PlateResult::rotated() { return std::move(rotated_); }

std::unique_ptr<Plate> const& PlateResult::original() const {
  return original_;
}

std::unique_ptr<Plate> const& PlateResult::rotated() const { return rotated_; }
}  // namespace c3picko
