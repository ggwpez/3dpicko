/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "ImageRecognition/algorithm_manager.h"

#include <QThreadPool>

#include "ImageRecognition/algorithm_job.h"
#include "ImageRecognition/algorithm_result.h"

using namespace d3picko;

namespace d3picko {
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
	Algorithm::ID algo_id, AlgorithmJob::ID job_id,
	std::shared_ptr<AlgorithmResult> result, QJsonObject settings) {
  for (Algorithm* algo : algos_) {
	if (algo->id() == algo_id) {
	  Algorithm* new_algo = algo->cloneEmpty();
	  AlgorithmJob* job = new AlgorithmJob(job_id, new_algo, settings, result,
										   pool_, new_algo->maxMs(), nullptr);
	  return job;
	}
  }
  throw Exception("Could not find algorithm (id=" + algo_id + ")");
}

QList<Algorithm*> AlgorithmManager::algos() const { return algos_; }

}  // namespace d3picko
