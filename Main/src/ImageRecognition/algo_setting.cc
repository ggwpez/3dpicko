#include "ImageRecognition/algo_setting.h"
#include <QJsonArray>
#include "Main/exception.h"

namespace c3picko {
static QVariant empty_qv = QJsonValue();

AlgoSetting AlgoSetting::make_checkbox(ID id, QString name, QString description,
									   bool default_value,
									   QList<AlgoSetting> sub_settings,
									   QColor color) {
  return AlgoSetting(id, name, "checkbox", description, empty_qv, empty_qv,
					 empty_qv, {}, default_value, sub_settings, color);
}

AlgoSetting AlgoSetting::make_dropdown(AlgoSetting::ID id, QString name,
									   QString description, QVariantMap options,
									   QString default_option_index,
									   QColor color) {
  return AlgoSetting(id, name, "dropdown", description, empty_qv, empty_qv,
					 empty_qv, options, default_option_index, {}, color);
}

AlgoSetting AlgoSetting::make_rangeslider_double(
	AlgoSetting::ID id, QString name, QString description, double min,
	double max, double step, math::Range<double> default_value, QColor color) {
  return AlgoSetting(id, name, "rangeslider", description, min, max, step, {},
					 QVariant::fromValue(default_value), {}, color);
}

AlgoSetting AlgoSetting::make_slider_int(ID id, QString name,
										 QString description, int min, int max,
										 int step, int default_value,
										 QColor color) {
  return AlgoSetting(id, name, "slider", description, min, max, step, {},
					 default_value, {}, color);
}

AlgoSetting AlgoSetting::make_slider_double(ID id, QString name,
											QString description, double min,
											double max, double step,
											double default_value,
											QColor color) {
  return AlgoSetting(id, name, "slider", description, min, max, step, {},
					 default_value, {}, color);
}

AlgoSetting::AlgoSetting(AlgoSetting::ID id, QString name, QString type,
						 QString description, QVariant min, QVariant max,
						 QVariant step, QVariantMap options,
						 QVariant default_value,
						 QList<AlgoSetting> sub_settings, QColor color,
						 QVariant value)
	: id_(id),
	  name_(name),
	  type_(type),
	  description_(description),
	  min_(min),
	  max_(max),
	  step_(step),
	  options_(options),
	  default_value_(default_value),
	  sub_settings_(sub_settings),
	  color_(color),
	  value_(value) {
  if (type == "slider") {
	if (!min_.isValid() || !max_.isValid() || !default_value_.isValid())
	  throw Exception("AlgoSetting: All passed values must be non-null");

	if ((min_.userType() != max_.userType()) ||
		(max_.userType() != step_.userType()) ||
		(step_.userType() != default_value_.userType()))
	  throw Exception(
		  "AlgoSetting: All passed values must have the declared type");
  }
  // TODO else

  if (value_.isValid() && (value_.userType() != default_value_.userType()))
	throw Exception(
		"AlgoSetting: default_value should have always the same type as value");
}

AlgoSetting::ID AlgoSetting::id() const { return id_; }

AlgoSetting::ID AlgoSetting::type() const { return type_; }

QString AlgoSetting::name() const { return name_; }

QString AlgoSetting::description() const { return description_; }

QVariant AlgoSetting::minVariant() const { return min_; }

QVariant AlgoSetting::maxVariant() const { return max_; }

QVariant AlgoSetting::stepVariant() const { return step_; }

QVariant AlgoSetting::valueVariant() const { return value_; }

QVariant AlgoSetting::defaultValueVariant() const { return default_value_; }

QVariantMap const& AlgoSetting::options() const { return options_; }

QColor AlgoSetting::color() const { return color_; }

void AlgoSetting::addSubSetting(const AlgoSetting& sub) {
  sub_settings_.push_back(sub);
}

AlgoSetting AlgoSetting::subSetting(QString id) const {
	for (AlgoSetting const& setting : sub_settings_) {
	  if (setting.id() == id) return setting;
	}

	throw Exception("Could not find AlgoSetting (id=" + id + ")");
}

QList<AlgoSetting> AlgoSetting::subSettings() const
{
	return sub_settings_;
}

template <>
QJsonObject Marshalling::toJson(const AlgoSetting& value) {
  QJsonObject obj;
  QString link = "/wiki/index.html#" + value.name().toLower().replace(' ', '-');
  QString name = value.name();
  // Color the name, if the color is not default
  if (value.color() != AlgoSetting::DefaultColor)
	name = "<font color=\"" + value.color().name() + "\">" + name + "</font>";

  obj["id"] = value.id();
  obj["type"] = value.type();
  obj["name"] = name;
  obj["description"] = "";  // = "<a href=\"" + link + "\" target=\"_blank\"
							// rel=\"noopener\" >Wiki link</a>";
  obj["color"] = value.color().name();

  if (value.type() == "slider") {
	obj["defaultValue"] = QJsonValue::fromVariant(value.defaultValueVariant());
	obj["valueType"] = value.defaultValueVariant().typeName();
	obj["min"] = QJsonValue::fromVariant(value.minVariant());
	obj["max"] = QJsonValue::fromVariant(value.maxVariant());
	obj["step"] = QJsonValue::fromVariant(value.stepVariant());
  } else if (value.type() == "checkbox") {
	obj["defaultValue"] = QJsonValue::fromVariant(value.defaultValueVariant());
	obj["valueType"] = value.defaultValueVariant().typeName();

	if (!value.subSettings().empty()) {
	  QJsonArray subs;
	  for (AlgoSetting const& sub : value.subSettings())
		subs.push_back(Marshalling::toJson(sub));

	  obj["conditional_settings"] = subs;
	}
  } else if (value.type() == "dropdown") {
	obj["defaultValue"] = value.defaultValueVariant().toString();
	obj["options"] = QJsonObject::fromVariantMap(value.options());
  } else if (value.type() == "rangeslider") {
	math::Range<double> def = value.defaultValue<math::Range<double>>();

	// TODO is this legit?
	if (!def.lower_closed_) def.lower_ += value.step<double>();
	if (!def.upper_closed_) def.upper_ -= value.step<double>();

	obj["defaultValue"] = QJsonObject{{"min", def.lower_}, {"max", def.upper_}};
	obj["min"] = QJsonValue::fromVariant(value.minVariant());
	obj["max"] = QJsonValue::fromVariant(value.maxVariant());
	obj["valueType"] = "double";  // FIXME
	obj["step"] = QJsonValue::fromVariant(value.stepVariant());
  } else
	Q_UNREACHABLE();

  return obj;
}

template <>
Q_DECL_DEPRECATED AlgoSetting Marshalling::fromJson(const QJsonObject& obj) {
  throw Exception("Marshalling::fromJson<AlgoSetting> not tested");  // TODO
  AlgoSetting::ID id = Marshalling::fromJson<QString>(obj["id"]);
  QString type = Marshalling::fromJson<QString>(obj["type"]).toLower();
  QString name = Marshalling::fromJson<QString>(obj["name"]);
  QString description = Marshalling::fromJson<QString>(obj["description"]);
  QVariant default_value = obj["defaultValue"].toVariant();
  QString value_type =
	  Marshalling::fromJson<QString>(obj["valueType"]).toLower();
  // color missing
  if (value_type != "float" && value_type != "double" && value_type != "int" &&
	  value_type != "bool")
	throw Exception("Unknown value type");

  if (type == "slider") {
	QString value_type = Marshalling::fromJson<QString>(obj["valueType"]);
	QVariant min = obj["min"].toVariant(), max = obj["max"].toVariant(),
			 step = obj["step"].toVariant();

	return AlgoSetting(id, name, "slider", description, min, max, step, {},
					   default_value);
  } else if (type == "checkbox") {
	if (!default_value.canConvert<bool>())
	  throw Exception(
		  "Could not convert default value for checkbox to boolean");

	return AlgoSetting(id, name, "checkbox", description, empty_qv, empty_qv,
					   empty_qv, {}, default_value);
  } else if (type == "dropdown") {
	QVariantMap options = obj["options"].toObject().toVariantMap();

	if (!default_value.canConvert<int>())
	  throw Exception("Could not convert default value for dropdown to int");

	return AlgoSetting(id, name, "dropdown", description, empty_qv, empty_qv,
					   empty_qv, options, default_value);
  } else if (type == "rangeslider") {
  } else
	throw Exception("Parsing exception");
}

const QColor AlgoSetting::DefaultColor = QColor::fromRgb(0, 0, 0, 0);
}  // namespace c3picko
