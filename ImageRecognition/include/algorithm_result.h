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
 * @brief Represents a failed or succeeded AlgorithmJob.
 */
class AlgorithmResult
{
  public:
	typedef QString ID;

	AlgorithmResult(ID id);
	virtual ~AlgorithmResult();

	/**
	 * @brief Will be called after the last step of the algorithm succeeded
	 */
	virtual void finalize();

  public slots:
	void cleanup();

  private:
	virtual void cleanup_impl();

  public:
	ID		  id_;
	QDateTime created_;
	/**
	 * @brief Explicit cleanup for debugging
	 */

	/**
	 * @brief The job succeeded
	 */
	bool stages_succeeded_;
	/**
	 * @brief The cleanup stage succeeded
	 */
	bool cleanup_succeeded_;

	/**
	 * @brief Last stage that the job was in.
	 */
	int		last_stage_;
	QString stage_error_, cleanup_error_;
	bool	is_finalized_ = false;

	ID		  id() const;
	bool	  stagesSucceeded() const;
	bool	  cleanupSucceeded() const;
	int		  lastStage() const;
	quint64   tookNs() const;
	QString   cleanupError() const;
	QString   stageError() const;
	QDateTime created() const;
};
MAKE_MARSHALLABLE(AlgorithmResult);
} // namespace c3picko
