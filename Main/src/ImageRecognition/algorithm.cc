#include "ImageRecognition/algorithm.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QThread>
#include "ImageRecognition/algo_setting.h"
#include "ImageRecognition/algorithm_job.h"
#include "ImageRecognition/algorithm_result.h"
#include "Main/exception.h"

namespace c3picko {

Algorithm::~Algorithm() {}

void Algorithm::run() {
  AlgorithmJob* j = job();
  if (!j) throw Exception("Algorithm always needs AlgorithmJob as parent");

  AlgorithmResult* result = j->result().get();

  QString error_prefix, error_infix, error_postfix, error;
  QTextStream error_ts(&error);
  QTextStream(&error_prefix)
	  << "Algorithm " << this->metaObject()->className() << " crashed (step=";
  QTextStream(&error_postfix)
	  << ",job=" << j->id() << ",thread=" << QThread::currentThreadId() << ") ";

  j->timeStart();
  int i;
  try {
	for (i = 0; i < steps_.size(); ++i) {
	  result->last_stage_ = i;
	  if (j->elapsedMs() >= j->maxMs())
		throw Exception("Job timed out (id=" + j->id() + ")");

	  steps_[i](j, result);  // TODO only pass job
	  result->stages_succeeded_ = true;
	}
	result->finalize();
  } catch (std::exception const& e) {
	error_ts << qPrintable(error_prefix) << i << qPrintable(error_postfix)
			 << e.what();
	result->stages_succeeded_ = false;
  } catch (...)  // FIXME abort
  {
	error_ts << qPrintable(error_prefix) << i << qPrintable(error_postfix)
			 << "unknown";
	result->stages_succeeded_ = false;
  }

  j->timeStop();
  if (result->stages_succeeded_)
	emit j->OnAlgoSucceeded();
  else
	emit j->OnAlgoFailed();

  if (error.size()) qCritical() << error;

  emit j->OnFinished();
}

Algorithm::Algorithm(Algorithm::ID id, QString name, QString description,
					 QList<AlgoStep> steps, QList<AlgoSetting> settings,
					 bool is_threadable, qint64 max_ms)
	: id_(id),
	  name_(name),
	  description_(description),
	  steps_(steps),
	  default_settings_(settings),
	  is_threadable_(is_threadable),
	  max_ms_(max_ms) {}

typename Algorithm::ID Algorithm::id() const { return id_; }

QString Algorithm::name() const { return name_; }

QString Algorithm::description() const { return description_; }

QList<AlgoSetting> Algorithm::defaultSettings() const {
  return default_settings_;
}

bool Algorithm::isThreadable() const { return is_threadable_; }

AlgorithmJob* Algorithm::job() const {
  return qobject_cast<AlgorithmJob*>(this->parent());
}

qint64 Algorithm::maxMs() const { return max_ms_; }

template <>
QJsonObject Marshalling::toJson(const Algorithm& value) {
  QJsonObject obj;
  obj["name"] = value.name();
  obj["description"] = value.description();

  QJsonArray json_settings;
  for (AlgoSetting const& setting : value.defaultSettings())
	json_settings.push_back(Marshalling::toJson(setting));  // NOTE add index to
															// support formats
															// that dont have
															// ordered arrays

  obj["settings"] = json_settings;

  return obj;
}
}  // namespace c3picko
