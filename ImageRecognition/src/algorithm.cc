#include "include/algorithm.h"
#include "include/algo_setting.h"
#include <QJsonArray>

namespace c3picko {

Algorithm::~Algorithm() {}

Algorithm::Algorithm(Algorithm::ID id, QString name, QString description,
                     QList<AlgoSetting> settings)
    : id_(id), name_(name), description_(description), settings_(settings) {}

Algorithm::ID Algorithm::id() const { return id_; }

QString Algorithm::name() const { return name_; }

QString Algorithm::description() const { return description_; }

QList<AlgoSetting> Algorithm::settings() const { return settings_; }

const AlgoSetting &Algorithm::settingById(AlgoSetting::ID id) const {
  for (AlgoSetting const &setting : settings_) {
    if (setting.id() == id)
      return setting;
  }

  throw std::runtime_error("Could not find AlgoSetting (by id)");
}

const AlgoSetting &Algorithm::settingByName(QString name) const {
  for (AlgoSetting const &setting : settings_) {
    if (setting.name() == name)
      return setting;
  }

  throw std::runtime_error("Could not find AlgoSetting (by name)");
}

void Algorithm::setSettingsValueByID(AlgoSetting::ID id, QVariant value) {
  for (AlgoSetting &setting : settings_) {
    if (setting.id() == id) {
      setting.setValue(value);
      return;
    }
  }

  qWarning() << "Ignoring AlgoSetting id=" << id;
}

void Algorithm::setSettingsValueByName(QString name, QVariant value) {
  for (AlgoSetting &setting : settings_) {
    if (setting.name() == name) {
      setting.setValue(value);
      return;
    }
  }

  qWarning() << "Ignoring AlgoSetting name=" << name;
}

template <> QJsonObject Marshalling::toJson(const Algorithm &value) {
  QJsonObject obj;

  obj["name"] = value.name();
  obj["description"] = value.description();

  QJsonObject json_settings;
  for (AlgoSetting const &setting : value.settings())
    json_settings[setting.id()] = Marshalling::toJson(setting);

  obj["settings"] = json_settings;

  return obj;
}
}
