#pragma once

#include <QElapsedTimer>
#include <QObject>
#include "include/algo_setting.h"
#include "include/algorithm_result.h"

class QThreadPool;
namespace c3picko {
class Algorithm;
class AlgorithmResult;
class AlgorithmManager;
/**
 * @brief Used by AlgorithmManager to encapsulate runnable Algorithms
 * potentially on different threads.
 * Obtain it with AlgorithmManager::createJob .
 */
class AlgorithmJob : public QObject {
  Q_OBJECT
  friend class AlgorithmManager;

 public:
  typedef QString ID;

 private:
  AlgorithmJob(ID id, Algorithm* algo, QJsonObject settings, void* input,
               AlgorithmResult* result, QThreadPool* pool, QObject* _parent);

 public:
  ~AlgorithmJob();

  void* input();
  AlgorithmResult* result() const;

  AlgoSetting const& settingById(AlgoSetting::ID id) const;
  AlgoSetting const& settingByName(QString name) const;

  void setSettingsValueByID(AlgoSetting::ID id, QJsonValue value);
  void setSettingsValueByName(QString name, QJsonValue value);

  QList<AlgoSetting> const& settings() const;
  void setSettings(QJsonObject const&);

  ID id() const;
  AlgorithmResult::ID result_id() const;

  qint64 took_ms() const;

 signals:
  void OnAlgoSucceeded();
  void OnAlgoFailed();

  void OnCleanupSucceeded();
  void OnCleanupFailed();

  void OnFinished();

 public slots:
  /**
   * @brief Starts the job
   * @param Indicates whether the job should be executed in another thread or
   * not if permitted by the algorithm.
   * @param Indicates wheter the job should delete itself after the Algorithm
   * finished.
   */
  void start(bool threaded, bool delete_when_done);

  void timeStart();
  void timeStop();

 private:
  ID id_;
  Algorithm* algo_ = nullptr;
  QThreadPool* pool_;
  QList<AlgoSetting> settings_;
  void* input_;
  /**
   * @brief This pointer is only valid during the call of start(...)
   * It will not be de/serialised. TODO
   */
  AlgorithmResult* result_;
  AlgorithmResult::ID result_id_;
  qint64 took_ms_;
  QElapsedTimer timer_;
};
MAKE_MARSHALLABLE(AlgorithmJob);
}  // namespace c3picko
