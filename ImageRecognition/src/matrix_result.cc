#include "include/matrix_result.h"

namespace c3picko
{
MatrixResult::MatrixResult(AlgorithmResult&& base) : AlgorithmResult(std::move(base)) {}

cv::Mat& MatrixResult::newMat()
{
	cv::Mat* new_mat = new cv::Mat();

	stack_.push_back(new_mat);

	return *new_mat;
}

cv::Mat& MatrixResult::newMat(cv::Mat const& copy_from)
{
	cv::Mat* new_mat = new cv::Mat();

	copy_from.copyTo(*new_mat);

	stack_.push_back(new_mat);
	return *new_mat;
}

cv::Mat& MatrixResult::oldMat()
{
	if (!stack_.size() || !stack_.back())
		throw Exception("Assertion failure: Stack empty");
	return *stack_.back();
}

const cv::Mat& MatrixResult::oldMat() const
{
	if (!stack_.size() || !stack_.back())
		throw Exception("Assertion failure: Stack empty");
	return *stack_.back();
}

void MatrixResult::cleanup_impl()
{
	qDebug() << "MatrixResult::cleanup_impl()";

	qDeleteAll(stack_);
	stack_.clear();
}

const std::list<cv::Mat*>& MatrixResult::stack() const { return stack_; }
}
