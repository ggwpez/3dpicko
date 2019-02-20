#include "include/algorithm_job.h"
#include "include/algorithm.h"
#include "include/algorithm_result.h"
#include <QVariantMap>

namespace c3picko
{

AlgorithmJob::AlgorithmJob(Algorithm* algo, QJsonObject settings, void* input, AlgorithmResult* result, QThreadPool* pool,
						   QObject* _parent)
	: QObject(_parent), algo_(algo), pool_(pool), result_(result)
{
	algo_->setParent(this);
	algo_->setAutoDelete(false);
	algo_->setSettings(settings);
	algo_->setPointers(input, result);

	// QObject::connect(result, &AlgorithmResult::OnFinished, this, [this, result]() { emit this->OnFinished(result); });
	// QObject::connect(result, &Algorithm::OnFinished, this, [this]() { qDebug() << this << "took" << timer_.elapsed() << "ms"; });
}

AlgorithmJob::~AlgorithmJob() { /* algo_ deleted as child element */}

AlgorithmResult* AlgorithmJob::result() const { return result_; }
}
