#include "include/algorithm.h"
#include "include/algo_setting.h"
#include <QJsonArray>
#include <QThread>

namespace c3picko
{

Algorithm::~Algorithm() {}

void Algorithm::setInput(void* input)
{
	Q_ASSERT(input);

	input_ = input;
}

void Algorithm::run()
{
	void* input  = input_;
	void* output = nullptr;

	QString error_prefix, error_infix, error_postfix;
	QTextStream(&error_prefix) << "Algorithm " << name_ << " (" << this->metaObject()->className() << ")"
							   << " crashed (step=";
	QTextStream(&error_postfix) << ",job=" << this->parent() << ",thread=" << QThread::currentThreadId() << ")";

	for (int i = 0; i < steps_.size(); ++i)
	{
		// Lets format an error string, in case it crashes

		try
		{
			steps_[i](this, input, &output);
		}
		catch (std::exception const& e)
		{
			qCritical("%s%d%s: %s", qPrintable(error_prefix), i, qPrintable(error_postfix), e.what());
		}
		catch (...) // FIXME abort
		{
			qCritical("%s%d%s: %s", qPrintable(error_prefix), i, qPrintable(error_postfix), "unknown");
		}

		input = output;
	}

	emit OnFinished(output);

	try
	{
		cleanup_(this);
	}
	catch (std::exception const& e)
	{
		qCritical("%s%s%s: %s", qPrintable(error_prefix), "cleanup", qPrintable(error_postfix), e.what());
	}
	catch (...)
	{
		qCritical("%s%s%s: %s", qPrintable(error_prefix), "cleanup", qPrintable(error_postfix), "unknown");
	}
}

Algorithm::Algorithm(Algorithm::ID id, QString name, QString description, QList<AlgoStep> steps, AlgoCleanup cleanup,
					 QList<AlgoSetting> settings, bool is_threadable)
	: id_(id), name_(name), description_(description), steps_(steps), cleanup_(cleanup), settings_(settings), is_threadable_(is_threadable)
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

void Algorithm::setSettingsValueByID(AlgoSetting::ID id, QVariant value)
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

void Algorithm::setSettingsValueByName(QString name, QVariant value)
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

QVector<void*>& Algorithm::stack() { return stack_; }

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
