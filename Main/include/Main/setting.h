/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QString>
#include <QVariantMap>
#include <vector>

namespace d3picko {
/**
 * @brief Represents a setting that will be displayed in the Web GUI.
 */
class Setting {
 public:
  typedef QString ID;
  enum class Type {
	TEXT,
	CHECKBOX,
	DROPDOWN,

	NUMBER,
	POINT2,
	POINT3,

	SINGLE_SLIDER,
	DOUBLE_SLIDER
  };

  enum class ValueType { INT, DOUBLE };

  ID id() const;
  QString name() const;
  Type type() const;
  QString description() const;
  ValueType value_type() const;

  Setting(Setting::ID id, QString name, Type type, QString description,
		  QVariant default_value, QString placeholder, QVariant min,
		  QVariant max, QVariant step, ValueType value_type,
		  QVariantMap options, int max_str_len, int min_str_len,
		  const std::vector<Setting>& sub_settings);

  void setValue(QJsonValue const& value);

 protected:
  // General attributes
  ID id_;
  QString name_;
  Type type_;
  QString description_;
  QVariant default_value_;
  QString placeholder_;

  /**
   * @brief SLIDER, NUMBER and POINT have min, max values and a step
   */
  QVariant min_, max_, step_;
  ValueType value_type_;

  /**
   * @brief Possible values for dropdowns
   */
  QVariantMap options_;

  /**
   * @brief TEXT can restrict the string length
   */
  int max_str_len_, min_str_len_;

  /**
   * @brief All types have this.
   * SLIDER have a math::Range .
   * DROPDOWN have a QString indexing options_
   */
  QVariant value_;

  /**
   * @brief Child settings
   */
  std::vector<Setting> sub_settings_;
};
}  // namespace d3picko
