/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Main/setting.h"

#include "Gcode/point.h"
#include "Main/exception.h"
#include "Main/marshalling.h"

namespace c3picko {

Setting::ID Setting::id() const { return id_; }

QString Setting::name() const { return name_; }

Setting::Type Setting::type() const { return type_; }

QString Setting::description() const { return description_; }

Setting::ValueType Setting::value_type() const { return value_type_; }

Setting::Setting(Setting::ID id, QString name, Setting::Type type,
				 QString description, QVariant default_value,
				 QString placeholder, QVariant min, QVariant max, QVariant step,
				 ValueType value_type, QVariantMap options, int max_str_len,
				 int min_str_len, const std::vector<Setting>& sub_settings)
	: id_(id),
	  name_(name),
	  type_(type),
	  description_(description),
	  default_value_(default_value),
	  placeholder_(placeholder),
	  min_(min),
	  max_(max),
	  step_(step),
	  value_type_(value_type),
	  options_(options),
	  max_str_len_(max_str_len),
	  min_str_len_(min_str_len),
	  sub_settings_(sub_settings) {}

void Setting::setValue(const QJsonValue& value) {
  switch (type_) {
	case Type::TEXT:
	  value_ = Marshalling::fromJson<QString>(value);
	  break;
	case Type::CHECKBOX: {
	  if (!value.toVariant().canConvert<bool>())
		throw Exception(Q_FUNC_INFO,
						"Expected bool but got " + value.toString());
	  else
		value_ = value.toBool();

	  break;
	}
	case Type::DROPDOWN: {
	  QString option = value.toString();

	  if (!options_.contains(option))  // No need for HTML-escaping here
		throw Exception(Q_FUNC_INFO, "Option unknown " + option);
	  else
		value_ = value;

	  break;
	}

	case Type::NUMBER: {
	  QVariant var_value = value.toVariant();

	  if (value_type_ == ValueType::INT) {
		if (!var_value.canConvert<qint64>())
		  throw Exception(Q_FUNC_INFO,
						  "Expected qint64 but got " + value.toString());
		else
		  value_ = var_value.toLongLong();
	  } else if (value_type_ == ValueType::DOUBLE) {
		if (!var_value.canConvert<double>())
		  throw Exception(Q_FUNC_INFO,
						  "Expected double but got " + value.toString());
		else
		  value_ = var_value.toDouble();
	  } else
		throw Exception(Q_FUNC_INFO, "Unreachable");

	  break;
	}
	case Type::POINT2: {
	  QJsonObject json = value.toObject();

	  if (!json.contains("x") || !json.contains("y"))
		throw Exception(Q_FUNC_INFO,
						"Expected Point2 but got " + value.toString());
	  else
		  // value_ = QVariant::fromValue(Marshalling::fromJson<Point>(json));

		  if (json.contains("z"))
		qWarning() << "Ignoring z parameter on Point2";

	  break;
	}
	case Type::POINT3: {
	  QJsonObject json = value.toObject();

	  if (!json.contains("x") || !json.contains("y") || !json.contains("z"))
		throw Exception(Q_FUNC_INFO,
						"Expected Point3 but got " + value.toString());
	  else
		// value_ = QVariant::fromValue(Marshalling::fromJson<Point>(json));

		break;
	}

	case Type::SINGLE_SLIDER: {
	  break;
	}
	case Type::DOUBLE_SLIDER:
	  break;

	default:
	  throw Exception(Q_FUNC_INFO, "Unreachable");
  }
}
}  // namespace c3picko
