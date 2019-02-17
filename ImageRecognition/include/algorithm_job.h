#pragma once

#include <QElapsedTimer>
#include <QObject>

class QThreadPool;
namespace c3picko
{
class AlgorithmManager;
class Algorithm;
/**
 * @brief Used by AlgorithmManager to encapsulate runnable Algorithms potentially on different threads.
 * Obtain it with AlgorithmManager::createJob .
 */
class AlgorithmJob : public QObject
{
	Q_OBJECT
	friend class AlgorithmManager;

  private:
	AlgorithmJob(Algorithm* algo, QThreadPool* pool, QObject* _parent);

  public:
	~AlgorithmJob();

  public slots:
	/**
	 * @brief Starts the job
	 * @param Indicates whether the job should be executed in another thread or
	 * not.
	 * @param Indicates wheter the job should delete itself after the Algorithm
	 * finished.
	 */
	void start(bool threaded, bool delete_when_done);

  signals:
	void OnFinished(void*);

  private:
	QElapsedTimer timer_;
	Algorithm*	algo_ = nullptr;
	QThreadPool*  pool_;
};
}
