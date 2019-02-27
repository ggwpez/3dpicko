#include "include/algo_setting.h"

namespace c3picko {
static QVariant empty_qv = QJsonValue();

AlgoSetting AlgoSetting::make_checkbox(ID id, QString name, QString description,
                                       bool default_value) {
  return AlgoSetting(id, name, "checkbox", description, empty_qv, empty_qv,
                     empty_qv, {}, default_value);
}

AlgoSetting AlgoSetting::make_dropdown(AlgoSetting::ID id, QString name,
                                       QString description, QVariantMap options,
                                       QString default_option_index) {
  return AlgoSetting(id, name, "dropdown", description, empty_qv, empty_qv,
                     empty_qv, options, default_option_index);
}

AlgoSetting AlgoSetting::make_rangeslider_double(
    AlgoSetting::ID id, QString name, QString description, double min,
    double max, double step, math::Range<double> default_value) {
  return AlgoSetting(id, name, "rangeslider", description, min, max, step, {},
                     QVariant::fromValue(default_value));
}

AlgoSetting AlgoSetting::make_slider_int(ID id, QString name,
                                         QString description, int min, int max,
                                         int step, int default_value) {
  return AlgoSetting(id, name, "slider", description, min, max, step, {},
                     default_value);
}

AlgoSetting AlgoSetting::make_slider_double(ID id, QString name,
                                            QString description, double min,
                                            double max, double step,
                                            double default_value) {
  return AlgoSetting(id, name, "slider", description, min, max, step, {},
                     default_value);
}

AlgoSetting::AlgoSetting(AlgoSetting::ID id, QString name, QString type,
                         QString description, QVariant min, QVariant max,
                         QVariant step, QVariantMap options,
                         QVariant default_value, QVariant value)
    : id_(id),
      name_(name),
      type_(type),
      description_(description),
      min_(min),
      max_(max),
      step_(step),
      options_(options),
      default_value_(default_value),
      value_(value) {
  if (type == "slider") {
    if (!min_.isValid() || !max_.isValid() || !default_value_.isValid())
      throw std::runtime_error(
          "AlgoSetting: All passed values must be non-null");

    if ((min_.userType() != max_.userType()) ||
        (max_.userType() != step_.userType()) ||
        (step_.userType() != default_value_.userType()))
      throw std::runtime_error(
          "AlgoSetting: All passed values must have the declared type");
  }
  // TODO else

  if (value_.isValid() && (value_.userType() != default_value_.userType()))
    throw std::runtime_error(
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

template <>
QJsonObject Marshalling::toJson(const AlgoSetting& value) {
  QJsonObject obj;
  QString link = "/wiki/index.html#" + value.name().toLower().replace(' ', '-');

  obj["id"] = value.id();
  obj["type"] = value.type();
  obj["name"] = value.name();
  obj["description"] =
      "<a href=\"" + link + "\" target=\"_blank\">Wiki link</a>";

  if (value.type() == "slider") {
    obj["defaultValue"] = QJsonValue::fromVariant(value.defaultValueVariant());
    obj["valueType"] = value.defaultValueVariant().typeName();
    obj["min"] = QJsonValue::fromVariant(value.minVariant());
    obj["max"] = QJsonValue::fromVariant(value.maxVariant());
    obj["step"] = QJsonValue::fromVariant(value.stepVariant());
  } else if (value.type() == "checkbox") {
    obj["defaultValue"] = QJsonValue::fromVariant(value.defaultValueVariant());
    obj["valueType"] = value.defaultValueVariant().typeName();
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
    obj["valueType"] = value.defaultValueVariant().typeName();
    obj["step"] = QJsonValue::fromVariant(value.stepVariant());
  } else
    Q_UNREACHABLE();

  return obj;
}

template <>
Q_DECL_DEPRECATED AlgoSetting Marshalling::fromJson(const QJsonObject& obj) {
  throw std::runtime_error(
      "Marshalling::fromJson<AlgoSetting> not tested");  // TODO
  AlgoSetting::ID id = obj["id"].toString();
  QString type = obj["type"].toString().toLower();
  QString name = obj["name"].toString();
  QString description = obj["description"].toString();
  QVariant default_value = obj["defaultValue"].toVariant();
  QString value_type = obj["valueType"].toString().toLower();

  if (value_type != "float" && value_type != "double" && value_type != "int" &&
      value_type != "bool")
    throw std::runtime_error("Unknown value type");

  if (type == "slider") {
    QString value_type = obj["valueType"].toString();
    QVariant min = obj["min"].toVariant(), max = obj["max"].toVariant(),
             step = obj["step"].toVariant();

    return AlgoSetting(id, name, "slider", description, min, max, step, {},
                       default_value);
  } else if (type == "checkbox") {
    if (!default_value.canConvert<bool>())
      throw std::runtime_error(
          "Could not convert default value for checkbox to boolean");

    return AlgoSetting(id, name, "checkbox", description, empty_qv, empty_qv,
                       empty_qv, {}, default_value);
  } else if (type == "dropdown") {
    QVariantMap options = obj["options"].toObject().toVariantMap();

    if (!default_value.canConvert<int>())
      throw std::runtime_error(
          "Could not convert default value for dropdown to int");

    return AlgoSetting(id, name, "dropdown", description, empty_qv, empty_qv,
                       empty_qv, options, default_value);
  } else if (type == "rangeslider") {
  } else
    throw std::runtime_error("Parsing exception");
}
}  // namespace c3picko
