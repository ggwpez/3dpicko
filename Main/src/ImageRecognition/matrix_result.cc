/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "ImageRecognition/matrix_result.h"

namespace d3picko {
MatrixResult::MatrixResult(AlgorithmResult&& base)
	: AlgorithmResult(std::move(base)) {}

MatrixResult::~MatrixResult() { cleanup(); }

cv::Mat& MatrixResult::first() {
  if (!stack_.size()) throw Exception("Assertion failure: Stack empty");
  return stack_.front();
}

const cv::Mat& MatrixResult::first() const {
  if (!stack_.size()) throw Exception("Assertion failure: Stack empty");
  return stack_.front();
}

cv::Mat& MatrixResult::newMat() {
  cv::Mat new_mat;

  stack_.push_back(new_mat);

  return stack_.back();
}

cv::Mat& MatrixResult::newMat(cv::Mat const& copy_from) {
  cv::Mat new_mat;

  copy_from.copyTo(new_mat);
  stack_.push_back(new_mat);

  return stack_.back();
}

cv::Mat& MatrixResult::oldMat() {
  if (!stack_.size()) throw Exception("Assertion failure: Stack empty");

  return stack_.back();
}

const cv::Mat& MatrixResult::oldMat() const {
  if (!stack_.size()) throw Exception("Assertion failure: Stack empty");
  return stack_.back();
}

void MatrixResult::cleanup() { stack_.clear(); }

const std::list<cv::Mat>& MatrixResult::stack() const { return stack_; }
}  // namespace d3picko
