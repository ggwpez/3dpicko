/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <memory>
#include <stack>

#include "ImageRecognition/algorithm_result.h"

namespace d3picko {
/**
 * @brief A stack based class to access all stages of an algorithm returning
 * cv::Mat
 */
class MatrixResult : public AlgorithmResult {
 public:
  using AlgorithmResult::AlgorithmResult;
  MatrixResult(AlgorithmResult&&);
  virtual ~MatrixResult();

  cv::Mat& first();
  cv::Mat const& first() const;
  cv::Mat& newMat();
  cv::Mat& newMat(cv::Mat const& copy_from);
  cv::Mat& oldMat();
  cv::Mat const& oldMat() const;

  void cleanup();
  std::list<cv::Mat> const& stack() const;

 protected:
  // MatrixResult is copyable
  std::list<cv::Mat> stack_;
};
}  // namespace d3picko
