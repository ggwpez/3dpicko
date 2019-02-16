#pragma once

#include <opencv2/core/core.hpp>

#include <QElapsedTimer>
#include <QRunnable>
#include <QThreadPool>

#include <string>

#include "conversion.h"
#include "detectionsettings.h"
#include "include/algo1_test.h"
#include "include/colony.hpp"
#include "include/colony_type.h"
#include "preprocessing.h"
#include "test.h"

namespace c3picko {

class AlgorithmJob : public QObject {
  Q_OBJECT
public:
  inline AlgorithmJob(Algorithm *algo, QThreadPool *pool, QObject *_parent)
      : QObject(_parent), algo_(algo), pool_(pool) {
    Q_ASSERT(algo_);
    algo_->setParent(this);
    QObject::connect(algo_, &Algorithm::OnFinished, this,
                     &AlgorithmJob::OnFinished);
    QObject::connect(algo_, &Algorithm::OnFinished, this, [this]() {
      qDebug() << this << "took" << timer_.elapsed() << "ms";
    });
    algo_->setAutoDelete(false);
  }
  inline ~AlgorithmJob() { /* algo_ deleted as child element */
  }

public slots:
  void start(bool threaded, bool delete_when_done);

signals:
  void OnFinished(void *);

private:
  QElapsedTimer timer_;
  Algorithm *algo_ = nullptr;
  QThreadPool *pool_;
};

/**
 * @file colonydetection.h
 * @author Mayer, Patrick
 * @brief The Colonydetection class is responsible for detecting colonies.
 */
class AlgorithmPipeline : public QObject {
  Q_OBJECT
public:
  AlgorithmPipeline(QThreadPool *pool, QList<Algorithm *> algos,
                    QObject *_parent = nullptr);

public:
  AlgorithmJob *createJob(cv::Mat source, Algorithm::ID id,
                          QJsonObject settings);
  QList<Algorithm *> algos() const;

  std::vector<Colony> colonies() const;
  std::vector<cv::Vec3f> pos() const;

protected:
  QThreadPool *pool_;
  QList<Algorithm *> algos_;
};
} // namespace c3picko
