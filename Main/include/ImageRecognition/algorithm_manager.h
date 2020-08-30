/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QObject>
#include <opencv2/core.hpp>

#include "ImageRecognition/algorithm.h"
#include "ImageRecognition/algorithm_job.h"

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
  AlgorithmJob* createJob(Algorithm::ID id, AlgorithmJob::ID job_id,
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
