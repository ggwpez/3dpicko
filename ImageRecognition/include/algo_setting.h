#pragma once

#include <QColor>
#include <QDebug>
#include <QPair>
#include <QString>
#include <QVariant>
#include "include/algorithms/helper.h"
#include "include/exception.h"
#include "include/marshalling.hpp"

namespace c3picko {
/**
 * @brief Represents a single option/threshold for an Algorithm.
 *
 * This class should be replaced with class Setting TODO
 */
class AlgoSetting {
 public:
  /**
   * @brief When setting color to this, the GUI decides.
   */
  static const QColor DefaultColor;
  typedef QString ID;

  static AlgoSetting make_checkbox(ID id, QString name, QString description,
                                   bool default_value,
                                   QList<AlgoSetting> sub_settings,
                                   QColor color = DefaultColor);
  static AlgoSetting make_dropdown(ID id, QString name, QString description,
                                   QStringList values, int default_value_index,
                                   QColor color = DefaultColor);
  static AlgoSetting make_slider_int(ID id, QString name, QString description,
                                     int min, int max, int step,
                                     int default_value,
                                     QColor color = DefaultColor);
  static AlgoSetting make_slider_double(ID id, QString name,
                                        QString description, double min,
                                        double max, double step,
                                        double default_value,
                                        QColor color = DefaultColor);
  static AlgoSetting make_dropdown(AlgoSetting::ID id, QString name,
                                   QString description, QVariantMap options,
                                   QString default_option_index,
                                   QColor color = DefaultColor);
  static AlgoSetting make_rangeslider_double(ID id, QString name,
                                             QString description, double min,
                                             double max, double step,
                                             math::Range<double> default_value,
                                             QColor color = DefaultColor);

  AlgoSetting(ID id, QString name, QString type, QString description,
              QVariant min, QVariant max, QVariant step, QVariantMap options,
              QVariant default_value, QList<AlgoSetting> sub_settings = {},
              QColor color = DefaultColor, QVariant value = QVariant());

 public:
  ID id() const;
  ID type() const;
  QString name() const;
  QString description() const;
  inline void setValue(QJsonValue const& value) {
    if (type_ == "rangeslider") {
      QJsonObject range = value.toObject();

      value_ = QVariant::fromValue(math::Range<double>(
          range["min"].toDouble(), range["max"].toDouble()));
    } else
      value_ = value;
  }
  // template <typename T> inline void setValue(T const& value) {
  // QVariant::fromValue(value); } // TODO check cast

  template <typename T>
  inline T value() const {
    if (!value_.canConvert<T>())
      return defaultValue<T>();
    else
      return qvariant_cast<T>(value_);
  }

  template <typename T>
  inline T min() const {
    if (!min_.canConvert<T>())
      throw Exception("Could not convert variant value");
    else
      return qvariant_cast<T>(min_);
  }

  template <typename T>
  inline T max() const {
    if (!max_.canConvert<T>())
      throw Exception("Could not convert variant value");
    else
      return qvariant_cast<T>(max_);
  }

  template <typename T>
  inline T step() const {
    if (!step_.canConvert<T>())
      throw Exception("Could not convert variant value");
    else
      return qvariant_cast<T>(step_);
  }

  template <typename T>
  inline T defaultValue() const {
    if (!default_value_.canConvert<T>())
      throw Exception("Could not convert variant value");
    else
      return qvariant_cast<T>(default_value_);
  }

  template <typename T>
  inline T option() const {
    if (!options_[value<QString>()].canConvert<T>())
      throw Exception("Could not convert variant value");
    else
      return qvariant_cast<T>(options_[value<QString>()]);
  }

  QVariant minVariant() const;
  QVariant maxVariant() const;
  QVariant stepVariant() const;
  QVariant valueVariant() const;
  QVariant defaultValueVariant() const;

  QVariantMap const& options() const;
  QColor color() const;

  void addSubSetting(AlgoSetting const& sub);
  QList<AlgoSetting> subSettings() const;

 protected:
  ID id_;
  QString name_, type_, description_;
  QVariant min_, max_, step_;
  QVariantMap options_;
  QVariant default_value_;
  /**
   * @brief Child settings, only usefull for checkboxes, where the checkbox
   * enables the Child settings.
   */
  QList<AlgoSetting> sub_settings_;
  QColor color_;

  QVariant value_;
};
MAKE_MARSHALLABLE(AlgoSetting);
}  // namespace c3picko
