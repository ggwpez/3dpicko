#pragma once

#include <QObject>
#include <opencv2/core.hpp>
#include "include/algorithm.h"
#include "include/algorithm_job.h"

class QThreadPool;
namespace c3picko {
/**
 * @brief Saves a List of algorithms, which can then be accessed by algos().
 */
class AlgorithmManager : public QObject {
  Q_OBJECT
 public:
  AlgorithmManager(QThreadPool* pool, QList<Algorithm*> algos,
                   QObject* _parent = nullptr);

 public:
  AlgorithmJob* createJob(cv::Mat source, Algorithm::ID id,
                          AlgorithmJob::ID job_id,
                          std::shared_ptr<AlgorithmResult> result,
                          QJsonObject settings);
  /**
   * @brief Always cloneEmpty() the result algorithm that you want to use.
   */
  QList<Algorithm*> algos() const;

 protected:
  QThreadPool* pool_;
  QList<Algorithm*> algos_;
};
}  // namespace c3picko
