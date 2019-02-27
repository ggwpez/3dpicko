#pragma once

#include <QObject>
#include <QRunnable>
#include <QVector>
#include <functional>
#include <stack>
#include "include/marshalling.hpp"

namespace c3picko {
class AlgorithmResult;
class AlgoSetting;
class AlgorithmJob;
/**
 * @brief Performs calculations stepwise.
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
            QList<AlgoSetting> defaultSettings, bool isThreadable);
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

  /**
   * @brief Abstract classes can not be marshallable.
   */
  QJsonObject baseToJson() const;

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
};
MAKE_SERIALIZABLE(Algorithm);
}  // namespace c3picko
