#include "include/algorithm_manager.h"
#include <QThreadPool>
#include "include/algorithm_job.h"
#include "include/algorithm_result.h"

using namespace c3picko;

namespace c3picko {
AlgorithmManager::AlgorithmManager(QThreadPool* pool, QList<Algorithm*> algos,
                                   QObject* _parent)
    : QObject(_parent), pool_(pool), algos_(algos) {
  for (Algorithm* algo : algos_) {
    algo->setParent(this);

#ifdef QT_NO_DEBUG
    for (auto it = algos.begin(); it != algos.end(); ++it)
      if (!(*it)->isThreadable())
        qWarning().nospace()
            << "All algorithms should be threadable in a release build (name="
            << (*it)->name() << ")";
#endif
  }
}

AlgorithmJob* AlgorithmManager::createJob(
    cv::Mat source, Algorithm::ID algo_id, AlgorithmJob::ID job_id,
    std::shared_ptr<AlgorithmResult> result, QJsonObject settings) {
  for (Algorithm* algo : algos_) {
    if (algo->id() == algo_id) {
      cv::Mat* input = new cv::Mat(source.clone());  // TODO needed?
      Algorithm* new_algo = algo->cloneEmpty();

      AlgorithmJob* job =
          new AlgorithmJob(job_id, new_algo, settings, input, result, pool_,
                           new_algo->maxMs(), nullptr);
      connect(job, &AlgorithmJob::OnFinished, job,
              [input]() {  // TODO do we need a context object here?
                delete input;
              });

      return job;
    }
  }

  return nullptr;
}

QList<Algorithm*> AlgorithmManager::algos() const { return algos_; }

}  // namespace c3picko
