#include "include/algorithm_manager.h"
#include "include/algorithm_job.h"
#include "include/algorithm_result.h"
#include <QThreadPool>

using namespace c3picko;

namespace c3picko
{
AlgorithmManager::AlgorithmManager(QThreadPool* pool, QList<Algorithm*> algos, QObject* _parent)
	: QObject(_parent), pool_(pool), algos_(algos)
{
	for (Algorithm* algo : algos_)
		algo->setParent(this);
}

AlgorithmJob* AlgorithmManager::createJob(cv::Mat source, Algorithm::ID algo_id, AlgorithmJob::ID job_id, AlgorithmResult* result,
										  QJsonObject settings)
{
	for (Algorithm* algo : algos_)
	{
		if (algo->id() == algo_id)
		{
			cv::Mat*   input	= new cv::Mat(source.clone());
			Algorithm* new_algo = algo->cloneEmpty();
			qint64	 max_time = new_algo->maxMs() * 2;

			AlgorithmJob* job = new AlgorithmJob(job_id, new_algo, settings, input, result, pool_, max_time, nullptr);
			connect(job, &AlgorithmJob::OnFinished, job, [input, job]() { // TODO do we need a context object here?
				delete input;
				job->deleteLater();
			});

			return job;
		}
	}

	return nullptr;
}

QList<Algorithm*> AlgorithmManager::algos() const { return algos_; }

} // namespace c3picko
