#include "include/algorithm.h"
#include "include/algo_setting.h"
#include "include/algorithm_job.h"
#include "include/algorithm_result.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QThread>

namespace c3picko
{

Algorithm::~Algorithm() {}

void Algorithm::setPointers(void* input, AlgorithmResult* output)
{
	Q_ASSERT(input);

	input_  = input;
	result_ = output;
}

void Algorithm::run()
{
	AlgorithmJob* job = qobject_cast<AlgorithmJob*>(this->parent());
	if (!job)
	{
		qCritical() << "Algorithm always needs AlgorithmJob as parent";
		return;
	}

	void* input = input_;

	QString		error_prefix, error_infix, error_postfix, error;
	QTextStream error_ts(&error);
	QTextStream(&error_prefix) << "Algorithm " << name_ << " (" << this->metaObject()->className() << ")"
							   << " crashed (step=";
	QTextStream(&error_postfix) << ",job=" << this->parent() << ",thread=" << QThread::currentThreadId() << ")";

	for (int i = 0; i < steps_.size(); ++i)
	{
		result_->last_stage_ = i;
		// Lets format an error string, in case it crashes

		try
		{
			steps_[i](this, result_);
			result_->stages_succeeded_ = true;
		}
		catch (std::exception const& e)
		{
			error_ts << qPrintable(error_prefix) << i << qPrintable(error_postfix) << e.what();
			result_->stages_succeeded_ = false;
			break;
		}
		catch (...) // FIXME abort
		{
			error_ts << qPrintable(error_prefix) << i << qPrintable(error_postfix) << "unknown";
			result_->stages_succeeded_ = false;
			break;
		}
	}

	try
	{
		result_->cleanup();
		result_->cleanup_succseeded_ = true;
	}
	catch (std::exception const& e)
	{
		qCritical("%s%s%s: %s", qPrintable(error_prefix), "cleanup", qPrintable(error_postfix), e.what());
		result_->cleanup_succseeded_ = false;
	}
	catch (...)
	{
		qCritical("%s%s%s: %s", qPrintable(error_prefix), "cleanup", qPrintable(error_postfix), "unknown");
		result_->cleanup_succseeded_ = false;
	}

	if (result_->stages_succeeded_)
		emit job->OnAlgoSucceeded();
	else
		emit job->OnAlgoFailed();

	if (result_->cleanup_succseeded_)
		emit job->OnCleanupSucceeded();
	else
		emit job->OnCleanupFailed();

	if (error.size())
		qCritical() << error;

	emit job->OnFinished();
}

Algorithm::Algorithm(Algorithm::ID id, QString name, QString description, QList<AlgoStep> steps, QList<AlgoSetting> settings,
					 bool is_threadable)
	: id_(id), name_(name), description_(description), steps_(steps), settings_(settings), is_threadable_(is_threadable)
{
}

typename Algorithm::ID Algorithm::id() const { return id_; }

QString Algorithm::name() const { return name_; }

QString Algorithm::description() const { return description_; }

QList<AlgoSetting> Algorithm::settings() const { return settings_; }

const AlgoSetting& Algorithm::settingById(AlgoSetting::ID id) const
{
	for (AlgoSetting const& setting : settings_)
	{
		if (setting.id() == id)
			return setting;
	}

	throw std::runtime_error("Could not find AlgoSetting (by id)");
}

const AlgoSetting& Algorithm::settingByName(QString name) const
{
	for (AlgoSetting const& setting : settings_)
	{
		if (setting.name() == name)
			return setting;
	}

	throw std::runtime_error("Could not find AlgoSetting (by name)");
}

void Algorithm::setSettingsValueByID(AlgoSetting::ID id, QJsonValue value)
{
	for (AlgoSetting& setting : settings_)
	{
		if (setting.id() == id)
		{
			setting.setValue(value);
			return;
		}
	}

	qWarning() << "Ignoring AlgoSetting id=" << id;
}

void Algorithm::setSettingsValueByName(QString name, QJsonValue value)
{
	for (AlgoSetting& setting : settings_)
	{
		if (setting.name() == name)
		{
			setting.setValue(value);
			return;
		}
	}

	qWarning() << "Ignoring AlgoSetting name=" << name;
}

void Algorithm::setSettings(const QJsonObject& sett)
{
	for (auto key : sett.keys())
		setSettingsValueByID(key, sett[key]);
}

AlgorithmResult* Algorithm::result() const { return result_; }

void* Algorithm::input() const { return input_; }

bool Algorithm::isThreadable() const { return is_threadable_; }

template <> QJsonObject Marshalling::toJson(const Algorithm& value)
{
	QJsonObject obj;

	obj["name"]		   = value.name();
	obj["description"] = value.description();

	QJsonObject json_settings;
	for (AlgoSetting const& setting : value.settings())
		json_settings[setting.id()] = Marshalling::toJson(setting);

	obj["settings"] = json_settings;

	return obj;
}
}
