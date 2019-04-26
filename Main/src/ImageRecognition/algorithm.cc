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
  AlgorithmJob* job = qobject_cast<AlgorithmJob*>(this->parent());
  if (!job) throw Exception("Algorithm always needs AlgorithmJob as parent");

  AlgorithmResult* result = job->result().get();

  QString error_prefix, error_infix, error_postfix, error;
  QTextStream error_ts(&error);
  QTextStream(&error_prefix)
	  << "Algorithm " << this->metaObject()->className() << " crashed (step=";
  QTextStream(&error_postfix)
	  << ",job=" << job->id() << ",thread=" << QThread::currentThreadId()
	  << ") ";

  job->timeStart();
  int i;
  try {
	for (i = 0; i < steps_.size(); ++i) {
	  result->last_stage_ = i;
	  if (job->elapsedMs() >= job->maxMs())
		throw Exception("Job timed out (id=" + job->id() + ")");

	  steps_[i](job, result);  // TODO only pass job
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

  job->timeStop();
  if (result->stages_succeeded_)
	emit job->OnAlgoSucceeded();
  else
	emit job->OnAlgoFailed();

  if (error.size()) qCritical() << error;

  emit job->OnFinished();
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