/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include "ImageRecognition/matrix_result.h"

namespace d3picko {
/**
 * @brief Represents the colonies that were detected by the detection
 * algorithms.
 */
class DetectionResult : public MatrixResult {
 public:
  using MatrixResult::MatrixResult;
  DetectionResult(AlgorithmResult&&);
  virtual ~DetectionResult() override;

  /**
   * @brief Partitions the colonies and sets included_end_ such that:
   * All colonies before included_end_ are included and all after and including
   * included_end_ are excluded.
   */
  virtual void finalize() override;

  std::vector<Colony>::iterator includedBegin();
  std::vector<Colony>::iterator includedEnd();

  std::vector<Colony>::iterator excludedBegin();
  std::vector<Colony>::iterator excludedEnd();

  std::vector<Colony> const& colonies() const;
  std::vector<Colony>& colonies();

 private:
  std::vector<Colony> colonies_;
  /**
   * @brief Separates the partitions between included and excluded colonies.
   * NOTE i used to save these as iterator, but moving or copying the iterator
   * seemed to break it.
   */
  std::size_t included_end_;
};
MAKE_MARSHALLABLE(DetectionResult);
}  // namespace d3picko
