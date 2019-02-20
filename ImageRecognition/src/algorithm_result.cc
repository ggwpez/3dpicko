#include "include/algorithm_result.h"
#include "include/colony.hpp"
#include <opencv2/core/mat.hpp>

namespace c3picko
{
AlgorithmResult::AlgorithmResult() : stages_succeeded_(false), cleanup_succseeded_(false) {}

AlgorithmResult::~AlgorithmResult() { cleanup(); }

cv::Mat& AlgorithmResult::newMat(cv::Mat const& copy_from)
{
	cv::Mat* new_mat = new cv::Mat();

	copy_from.copyTo(*new_mat);

	stack_.push_back(new_mat);
	return *new_mat;
}

cv::Mat& AlgorithmResult::oldMat()
{
	Q_ASSERT(stack_.back());
	return *stack_.back();
}

cv::Mat& AlgorithmResult::newMat()
{
	cv::Mat* new_mat = new cv::Mat();

	stack_.push_back(new_mat);

	return *new_mat;
}

void AlgorithmResult::cleanup()
{
	// Dont delete them right now, otherwise we cant user them
	// colonies_.~vector();
	qDeleteAll(stack_);
}
}
