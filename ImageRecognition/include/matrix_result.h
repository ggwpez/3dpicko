#pragma once

#include <memory>
#include <stack>
#include "include/algorithm_result.h"

namespace c3picko {
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
}  // namespace c3picko
