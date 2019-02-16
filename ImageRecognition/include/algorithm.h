#pragma once

#include "include/algo_setting.h"
#include <QRunnable>
#include <QVector>
#include <memory>

namespace c3picko {
class AlgoSetting;
/**
 * @brief Algorithm class
 * @tparam Input data type
 * @tparam Output data type
 */
class Algorithm : public QObject, public QRunnable {
  Q_OBJECT
public:
  typedef QString ID;

  Algorithm(ID id, QString name, QString description,
            QList<AlgoSetting> settings);
  virtual ~Algorithm() override;

  /**
   * @brief Call with according input data pointer before run()
   * @param input
   */
  void setInput(const void *input);
  /**
   * @brief Executes the algorithm. Emits OnFinished() when done.
   */
  virtual void run() override = 0;
  /**
   * @brief Returns a new instance of the derived class.
   * Function is similar to common clone() but returns an empty instance, hence
   * the name.
   * Used instead of a factory.
   * @return Pointer to instance.
   */
  virtual Algorithm *cloneEmpty() const = 0;

signals:
  void OnFinished(void *);

public:
  ID id() const;
  QString name() const;
  QString description() const;
  QList<AlgoSetting> settings() const;

  AlgoSetting const &settingById(AlgoSetting::ID id) const;
  AlgoSetting const &settingByName(QString name) const;

  void setSettingsValueByID(AlgoSetting::ID id, QVariant value);
  void setSettingsValueByName(QString name, QVariant value);

  void setSettings(QJsonObject const &);

  /**
   * @brief Abstract classes can not be marshallable.
   */
  QJsonObject baseToJson() const;

protected:
  const ID id_;
  const QString name_, description_;
  QList<AlgoSetting> settings_;
  // Algorithmic data
  const void *input_;
};
MAKE_SERIALIZABLE(Algorithm);
}
