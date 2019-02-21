#pragma once

#include <QElapsedTimer>
#include <QObject>

class QThreadPool;
namespace c3picko {
class Algorithm;
class AlgorithmResult;
class AlgorithmManager;
/**
 * @brief Used by AlgorithmManager to encapsulate runnable Algorithms
 * potentially on different threads.
 * Obtain it with AlgorithmManager::createJob .
 */
class AlgorithmJob : public QObject {
  Q_OBJECT
  friend class AlgorithmManager;

private:
  AlgorithmJob(Algorithm *algo, QJsonObject settings, void *input,
               AlgorithmResult *result, QThreadPool *pool, QObject *_parent);

public:
  ~AlgorithmJob();

  AlgorithmResult *result() const;

signals:
  void OnAlgoSucceeded();
  void OnAlgoFailed();

  void OnCleanupSucceeded();
  void OnCleanupFailed();

  void OnFinished();

public slots:
  /**
   * @brief Starts the job
   * @param Indicates whether the job should be executed in another thread or
   * not if permitted by the algorithm.
   * @param Indicates wheter the job should delete itself after the Algorithm
   * finished.
   */
  void start(bool threaded, bool delete_when_done);

private:
  Algorithm *algo_ = nullptr;
  QThreadPool *pool_;
  AlgorithmResult *result_;
};
}
