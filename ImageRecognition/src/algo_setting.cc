#include "include/algo_setting.h"

namespace c3picko {
static QVariant empty_qv = QVariant();

AlgoSetting AlgoSetting::make_checkbox(ID id, QString name, QString description,
                                       bool default_value) {
  return AlgoSetting(id, name, "checkbox", description, empty_qv, empty_qv,
                     empty_qv, {}, default_value);
}

AlgoSetting AlgoSetting::make_dropdown(AlgoSetting::ID id, QString name,
                                       QString description, QStringList values,
                                       int default_value_index) {
  return AlgoSetting(id, name, "dropdown", description, empty_qv, empty_qv,
                     empty_qv, values, default_value_index);
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
                         QVariant step, QStringList enum_values,
                         QVariant default_value, QVariant value)
    : id_(id), name_(name), type_(type), description_(description), min_(min),
      max_(max), step_(step), enum_values_(enum_values),
      default_value_(default_value), value_(value) {
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

QStringList const &AlgoSetting::enum_values() const { return enum_values_; }

template <> QJsonObject Marshalling::toJson(const AlgoSetting &value) {
  QJsonObject obj;

  obj["id"] = value.id();
  obj["type"] = value.type();
  obj["name"] = value.name();
  obj["description"] = value.description();

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
    auto const &enum_values = value.enum_values();
    QJsonObject json_values;

    for (int i = 0; i < enum_values.size(); ++i)
      json_values[QString::number(i)] = enum_values[i];

    obj["possible_values"] = json_values;
  } else
    Q_UNREACHABLE();

  return obj;
}

template <> AlgoSetting Marshalling::fromJson(const QJsonObject &obj) {
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
    QJsonObject values = obj["possible_values"].toObject();
    QStringList enum_values;
    for (auto key : values.keys()) {
      QString value = values[key].toString();

      if (value.isEmpty())
        qWarning() << "Ignoring empty enum value in fromJson<dropdown>";
      else
        enum_values << value;
    }

    if (!default_value.canConvert<int>())
      throw std::runtime_error(
          "Could not convert default value for dropdown to int");

    return AlgoSetting(id, name, "dropdown", description, empty_qv, empty_qv,
                       empty_qv, enum_values, default_value);
  } else
    throw std::runtime_error("Parsing exception");
}
}
