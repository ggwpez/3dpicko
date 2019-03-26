#pragma once

#include "include/algorithm_result.h"
#include <stack>

namespace c3picko
{
/**
 * @brief A stack based class to access all stages of an algorithm returning cv::Mat
 */
class MatrixResult : public AlgorithmResult
{
  public:
	using AlgorithmResult::AlgorithmResult;
	MatrixResult(AlgorithmResult&&);

	cv::Mat&	   newMat();
	cv::Mat&	   newMat(cv::Mat const& copy_from);
	cv::Mat&	   oldMat();
	cv::Mat const& oldMat() const;

	std::list<cv::Mat*> const& stack() const;

	void cleanup_impl() override;

  protected:
	std::list<cv::Mat*> stack_;
};
}
