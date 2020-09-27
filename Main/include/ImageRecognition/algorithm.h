/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QObject>
#include <QRunnable>
#include <QVector>
#include <functional>
#include <stack>

#include "Main/marshalling.h"

namespace d3picko {
class AlgorithmResult;
class AlgoSetting;
class AlgorithmJob;
/**
 * @brief Performs calculations stepwise in a pipeline way of excution.
 * Operations from other Algorithms can be reused.
 * Needs AlgorithmJob as parent QObject before calling run().
 * @tparam Input data type
 * @tparam Output data type
 */
class Algorithm : public QObject, public QRunnable {
  Q_OBJECT
 public:
  typedef QString ID;
  typedef void (*AlgoStep)(AlgorithmJob*, AlgorithmResult*);
  typedef void (*AlgoCleanup)(Algorithm*);

  Algorithm(ID id, QString name, QString description, QList<AlgoStep> steps,
			QList<AlgoSetting> defaultSettings, bool isThreadable,
			qint64 maxMs);
  virtual ~Algorithm() override;

  /**
   * @brief Executes all steps of algorithm one by one.
   * Emits OnFinished(void*) when done with the last output pointer.
   */
  virtual void run() override;

  /**
   * @brief Returns a new instance of the derived class.
   * Function is similar to common clone() but returns an empty instance, hence
   * the name.
   * Used instead of a factory.
   * @return Pointer to instance.
   */
  virtual Algorithm* cloneEmpty() const = 0;

 public:
  ID id() const;
  QString name() const;
  QString description() const;
  QList<AlgoSetting> defaultSettings() const;
  bool isThreadable() const;
  AlgorithmJob* job() const;

  /**
   * @brief Abstract classes can not be marshallable.
   */
  QJsonObject baseToJson() const;

  qint64 maxMs() const;

 protected:
  const ID id_;
  const QString name_, description_;
  /**
   * @brief Steps of the algorithm. Consecutively executed.
   */
  QList<AlgoStep> steps_;
  /**
   * @brief Settings needed supported by the algorithm.
   */
  const QList<AlgoSetting> default_settings_;
  bool is_threadable_;
  /**
   * @brief Estimated maximal time the algorithm should take in ms.
   * NOTE should this be a function taking a complexity value?
   */
  qint64 max_ms_;
};
MAKE_SERIALIZABLE(Algorithm);
}  // namespace d3picko
