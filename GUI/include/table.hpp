#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QObject>
#include <map>
#include <type_traits>
#include <utility>
#include "include/exception.h"
#include "include/json_constructable.hpp"
#include "include/json_convertable.h"
#include "include/marshalling.hpp"

namespace c3picko {
/**
 * A Database Table in the ordinary sense.
 * Sadly this is not a QObject since templates do not work with it.
 *
 * TODO addAsJson and getAsJson are probably buggy with Value=QJsonObject
 */
template <typename Value>
class Table : public JsonConvertable {
  // static_assert(std::is_base_of<JsonConstructable, Value>(),
  //            "Value type must implement interface JsonConvertable");

 public:
  typedef QString Key;
  typedef QMap<Key, Value> MapType;

  /**
   * @brief Add the value to the table and overrides old entries.
   * @param key
   * @param value
   */
  inline void add(Key const& key, Value const& value) {
    auto it = entries_.find(key);

    if (it != entries_.end()) {
      // *it = value;
      entries_.erase(it);
      entries_.insert(key, value);
    } else {
      entries_.insert(key, value);
    }
  }

  /**
   * @brief Adds an object to the table by deserialising it from json.
   * @param key
   * @param json Value
   */
  inline void addAsJson(Key const& key, QJsonObject const& json) {
    add(key, Marshalling::fromJson<Value>(json));
  }

  /**
   * @return Wether the key exist.
   */
  inline bool exists(Key const& key) const {
    return (entries_.find(key) != entries_.end());
  }

  /**
   * @brief get Throws if key does not exist.
   * @param key
   *
   * We can return a reference here, since QMap also does that
   * @throws When the object does not exist.
   */
  inline Value& get(Key const& key) {
    auto it = entries_.find(key);

    if (it == entries_.end())
      throw Exception("Table", "Key not found");
    else
      return it.value();
  }

  inline Value const& get(Key const& key) const {
    auto it = entries_.find(key);

    if (it == entries_.end())
      throw Exception("Table", "Key not found");
    else
      return it.value();
  }

  /**
   * @param key
   * @return Corresponding object for this key in Json format.
   */
  inline QJsonObject getAsJson(Key const& key) const {
    Value const& value = get(key);

    QJsonObject json;
    value.write(json);
    return json;
  }

  /**
   * @brief Removes the object for key key.
   * @param key
   * @thows does not throw except for Dtor Exceptions.
   */
  inline void remove(Key const& key) { entries_.remove(key); }

  // key_value_iterator was introduced in 5.10 but the CI-Server has 5.9.5
  inline typename MapType::iterator begin() { return entries_.begin(); }
  inline typename MapType::iterator end() { return entries_.end(); }

  inline typename MapType::const_iterator begin() const {
    return entries_.cbegin();
  }
  inline typename MapType::const_iterator end() const {
    return entries_.cend();
  }

 public:
  inline void read(QJsonObject const& obj) override {
    for (auto it = obj.begin(); it != obj.end(); ++it)
      addAsJson(it.key(), it.value().toObject());
  }

  inline void write(QJsonObject& obj) const override {
    for (auto it = entries_.begin(); it != entries_.end(); ++it)
      obj[it.key()] = Marshalling::toJson(it.value());
  }

  inline MapType const& entries() const { return entries_; }

  inline int size() const { return entries_.size(); }

 private:
  MapType entries_;
};
}  // namespace c3picko
