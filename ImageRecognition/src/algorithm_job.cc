#include "include/algorithm_job.h"
#include <QThreadPool>
#include <QVariantMap>
#include "include/algorithm.h"
#include "include/algorithm_result.h"

namespace c3picko {

AlgorithmJob::AlgorithmJob(AlgorithmJob::ID id, Algorithm *algo,
                           QJsonObject settings, void *input,
                           AlgorithmResult *result, QThreadPool *pool,
                           qint64 max_ms, QObject *_parent)
    : QObject(_parent),
      id_(id),
      algo_(algo),
      pool_(pool),
      input_(input),
      result_(result),
      result_id_(result->id()),
      max_ms_(max_ms),
      took_ms_(0) {
  algo_->setParent(this);
  algo_->setAutoDelete(false);

  // First we get the default settings from the Algorithm, so that in case
  // the user did not provide all settings, they are filled in with the
  // default values.
  settings_ = algo->defaultSettings();
  // Override the default values with the one from the user.
  setSettings(settings);

  // QObject::connect(result, &AlgorithmResult::OnFinished, this, [this,
  // result]() { emit this->OnFinished(result); });
  // QObject::connect(result, &Algorithm::OnFinished, this, [this]() { qDebug()
  // << this << "took" << timer_.elapsed() << "ms"; });
}

AlgorithmJob::~AlgorithmJob() { /* algo_ deleted as child element */
}

void *AlgorithmJob::input() { return input_; }

void AlgorithmJob::start(bool threaded, bool delete_when_done) {
  if (delete_when_done)
    connect(this, SIGNAL(OnFinished()), this, SLOT(deleteLater()));

  if (threaded && algo_->isThreadable())
    pool_->start(algo_);
  else
    algo_->run();
}

void AlgorithmJob::timeStart() { timer_.start(); }

void AlgorithmJob::timeStop() { took_ms_ = timer_.elapsed(); }

qint64 AlgorithmJob::maxMs() const { return max_ms_; }

qint64 AlgorithmJob::tookMs() const { return took_ms_; }

qint64 AlgorithmJob::elapsedMs() const { return timer_.elapsed(); }

AlgorithmResult::ID AlgorithmJob::result_id() const { return result_id_; }

AlgorithmJob::ID AlgorithmJob::id() const { return id_; }

const QList<AlgoSetting> &AlgorithmJob::settings() const { return settings_; }

const AlgoSetting &AlgorithmJob::settingById(AlgoSetting::ID id) const {
  for (AlgoSetting const &setting : settings_) {
    if (setting.id() == id) return setting;
  }

  throw Exception("Could not find AlgoSetting (id=" + id + ")");
}

const AlgoSetting &AlgorithmJob::settingByName(QString name) const {
  for (AlgoSetting const &setting : settings_) {
    if (setting.name() == name) return setting;
  }

  throw Exception("Could not find AlgoSetting (name=" + name + ")");
}

void AlgorithmJob::setSettingsValueByID(AlgoSetting::ID id, QJsonValue value) {
  for (AlgoSetting &setting : settings_) {
    if (setting.id() == id) {
      setting.setValue(value);
      return;
    }
  }

  qWarning() << "Ignoring AlgoSetting id=" << id;
}

void AlgorithmJob::setSettingsValueByName(QString name, QJsonValue value) {
  for (AlgoSetting &setting : settings_) {
    if (setting.name() == name) {
      setting.setValue(value);
      return;
    }
  }

  qWarning() << "Ignoring AlgoSetting name=" << name;
}

void AlgorithmJob::setSettings(const QJsonObject &sett) {
  for (auto key : sett.keys()) setSettingsValueByID(key, sett[key]);
}

AlgorithmResult *AlgorithmJob::result() const { return result_; }

template <>
QJsonObject Marshalling::toJson(const AlgorithmJob &value) {
  QJsonObject obj;

  obj["id"] = value.id();

  return obj;
}

// template <>
// AlgorithmJob Marshalling::fromJson(const QJsonObject &obj) {}
}  // namespace c3picko
