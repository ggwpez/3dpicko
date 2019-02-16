#pragma once

#include "include/json_constructable.hpp"
#include "include/json_convertable.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QObject>
#include <map>
#include <type_traits>
#include <utility>

namespace c3picko {
/**
 * Sadly this is not a QObject since templates do not work with it.
 *
 * TODO addAsJson and getAsJson are probably buggy with Value=QJsonObject
 */
template <typename Value> class Table : public JsonConvertable {

  static_assert(std::is_base_of<JsonConstructable, Value>(),
                "Value type must implement interface JsonConvertable");

public:
  typedef QString Key;
  typedef QMap<Key, Value> MapType;

  inline void add(Key const &key, Value const &value) {
    auto it = entries_.find(key);

    if (it != entries_.end()) {
      // *it = value;
      entries_.erase(it);
      entries_.insert(key, value);
    } else {
      entries_.insert(key, value);
    }
  }

  inline void addAsJson(Key const &key, QJsonObject const &json) {
    add(key, Value(json));
  }

  inline bool exists(Key const &key) const {
    return (entries_.find(key) != entries_.end());
  }

  /**
   * @brief get Throws if key does not exist.
   * @param key
   *
   * We can return a reference here, since QMap also does that
   */
  inline Value &get(Key const &key) {
    auto it = entries_.find(key);

    if (it == entries_.end())
      throw std::runtime_error("Key not found");
    else
      return *it;
  }

  inline QJsonObject getAsJson(Key const &key) const {
    Value const &value = get(key);

    QJsonObject json;
    value.write(json);
    return json;
  }

  inline void remove(Key const &key) { entries_.remove(key); }

  // key_value_iterator was introduced in 5.10 but the CI-Server has 5.9.5
  inline typename MapType::iterator begin() { return entries_.begin(); }
  inline typename MapType::iterator end() { return entries_.end(); }

public:
  inline void read(QJsonObject const &obj) override {
    for (auto it = obj.begin(); it != obj.end(); ++it)
      addAsJson(it.key(), it.value().toObject());
  }

  inline void write(QJsonObject &obj) const override {
    for (auto it = entries_.begin(); it != entries_.end(); ++it) {
      QJsonObject json;
      it.value().write(json);

      obj[it.key()] = json;
    }
  }

private:
  MapType entries_;
};
} // namespace c3picko
