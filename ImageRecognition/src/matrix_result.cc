#include "include/matrix_result.h"

namespace c3picko {
MatrixResult::MatrixResult(AlgorithmResult&& base)
    : AlgorithmResult(std::move(base)) {}

MatrixResult::~MatrixResult() {
  qDebug() << "MatrixResult::~MatrixResult()";
  cleanup();
}

cv::Mat& MatrixResult::first() const {
  if (!stack_.size() || !stack_.front())
    throw Exception("Assertion failure: Stack empty");
  return *stack_.front();
}

cv::Mat& MatrixResult::newMat() {
  auto new_mat = std::make_shared<cv::Mat>();

  stack_.push_back(new_mat);

  return *new_mat;
}

cv::Mat& MatrixResult::newMat(cv::Mat const& copy_from) {
  auto new_mat = std::make_shared<cv::Mat>();

  copy_from.copyTo(*new_mat);

  stack_.push_back(new_mat);
  return *new_mat;
}

cv::Mat& MatrixResult::oldMat() {
  if (!stack_.size() || !stack_.back())
    throw Exception("Assertion failure: Stack empty");
  return *stack_.back();
}

const cv::Mat& MatrixResult::oldMat() const {
  if (!stack_.size() || !stack_.back())
    throw Exception("Assertion failure: Stack empty");
  return *stack_.back();
}

void MatrixResult::cleanup() {
  qDebug() << "MatrixResult::cleanup()" << this;
  stack_.clear();
}

const std::list<std::shared_ptr<cv::Mat>>& MatrixResult::stack() const {
  return stack_;
}
}  // namespace c3picko
