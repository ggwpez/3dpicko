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

AlgorithmJob* AlgorithmManager::createJob(cv::Mat source, Algorithm::ID id, QJsonObject settings)
{
	for (Algorithm* algo : algos_)
	{
		if (algo->id() == id)
		{
			cv::Mat*		 input	= new cv::Mat(source.clone());
			Algorithm*		 new_algo = algo->cloneEmpty();
			AlgorithmResult* result   = new AlgorithmResult();

			AlgorithmJob* job = new AlgorithmJob(new_algo, settings, input, result, pool_, nullptr);
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

void AlgorithmJob::start(bool threaded, bool delete_when_done)
{
	if (delete_when_done)
		connect(this, SIGNAL(OnFinished()), this, SLOT(deleteLater()));

	if (threaded && algo_->isThreadable())
		pool_->start(algo_);
	else
		algo_->run();
}

} // namespace c3picko
