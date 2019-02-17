#include "include/algorithm_job.h"
#include "include/algorithm.h"

namespace c3picko
{

AlgorithmJob::AlgorithmJob(Algorithm* algo, QThreadPool* pool, QObject* _parent) : QObject(_parent), algo_(algo), pool_(pool)
{
	algo_->setParent(this);
	algo_->setAutoDelete(false);
	QObject::connect(algo_, &Algorithm::OnFinished, this, &AlgorithmJob::OnFinished);
	QObject::connect(algo_, &Algorithm::OnFinished, this, [this]() { qDebug() << this << "took" << timer_.elapsed() << "ms"; });
}

AlgorithmJob::~AlgorithmJob() { /* algo_ deleted as child element */}
}
