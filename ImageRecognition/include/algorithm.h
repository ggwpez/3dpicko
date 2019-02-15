#pragma once

#include "include/algo_setting.h"
#include <QVector>

namespace c3picko {
class AlgoSetting;
class Algorithm {

public:
  typedef QString ID;

  Algorithm(ID id, QString name, QString description,
            QList<AlgoSetting> settings);
  virtual ~Algorithm();

  virtual void run(void *const input, void *output) = 0;

  ID id() const;
  QString name() const;
  QString description() const;
  QList<AlgoSetting> settings() const;

  AlgoSetting const &settingById(AlgoSetting::ID id) const;
  AlgoSetting const &settingByName(QString name) const;

  void setSettingsValueByID(AlgoSetting::ID id, QVariant value);
  void setSettingsValueByName(QString name, QVariant value);

  /**
   * @brief Abstract classes can not be marshallable.
   */
  QJsonObject baseToJson() const;

protected:
  const ID id_;
  const QString name_, description_;
  QList<AlgoSetting> settings_;
};
MAKE_SERIALIZABLE(Algorithm);
}
