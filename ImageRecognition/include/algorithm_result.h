#pragma once

#include "include/colony.hpp"
#include <QString>
#include <vector>

namespace cv
{
class Mat;
}
namespace c3picko
{
class Colony;
/**
 * @brief Represents a failed or succeeded job.
 */
class AlgorithmResult : public QObject
{
	Q_OBJECT
  public:
	typedef QString ID;

	AlgorithmResult();
	~AlgorithmResult();

	cv::Mat& newMat();
	cv::Mat& newMat(cv::Mat const& copy_from);

	cv::Mat& oldMat();

  public:
	/**
	 * @brief Explicit cleanup for debugging
	 */
	void cleanup();

	/**
	 * @brief The job succeeded
	 */
	bool stages_succeeded_;
	bool cleanup_succseeded_;

	/**
	 * @brief Last stage that the job was in.
	 */
	int		last_stage_;
	QString error_string_;

	/**
	 * @brief Amount of nanoseconds the job took.
	 */
	quint64 took_ns_;

	struct
	{
	} plate_; // TODO
	std::vector<Colony> colonies_;

  private:
	std::list<cv::Mat*> stack_;
};
}
