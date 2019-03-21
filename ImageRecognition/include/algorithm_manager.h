#pragma once

#include "include/algorithm.h"
#include "include/algorithm_job.h"
#include <QObject>
#include <opencv2/core.hpp>

class QThreadPool;
namespace c3picko {
class AlgorithmManager : public QObject {
  Q_OBJECT
public:
  AlgorithmManager(QThreadPool *pool, QList<Algorithm *> algos,
                   QObject *_parent = nullptr);

public:
  AlgorithmJob *createJob(cv::Mat source, Algorithm::ID id,
                          AlgorithmJob::ID job_id, AlgorithmResult *result,
                          QJsonObject settings);
  QList<Algorithm *> algos() const;

protected:
  QThreadPool *pool_;
  QList<Algorithm *> algos_;
};
} // namespace c3picko
