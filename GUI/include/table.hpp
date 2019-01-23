#pragma once

#include <QObject>
#include <QMap>
#include <QJsonObject>
#include <QJsonValue>
#include <include/json_convertable.h>

namespace c3picko {
	/**
	 * Sadly this is not a QObject since templates do not work with it.
	 *
	 * TODO addAsJson and getAsJson are probably buggy with Value=QJsonObject
	 */
	template<typename Value>
	class Table : public JsonConvertable
	{
		static_assert (std::is_base_of<JsonConvertable, Value>(), "Value type must implement interface JsonConvertable");
	public:
		typedef QString Key;
		typedef QMap<Key, Value> MapType;

		inline void add(Key const& key, Value const& value)
		{
			auto it = entries_.find(key);

			if (it != entries_.end())
			{
				*it = value;
			}
			else
			{
				entries_.insert(key, value);
			}
		}

		inline void addAsJson(Key const& key, QJsonObject const& json)
		{
			Value value;
			value.read(json);

			add(key, value);
		}

		inline bool exists(Key const& key) const
		{
			return (entries_.find(key) != entries_.end());
		}

		/**
		 * @brief get Throws if key does not exist.
		 * @param key
		 *
		 * We can return a reference here, since QMap also does that
		 */
		inline Value const& get(Key const& key) const
		{
			auto it = entries_.find(key);

			if (it == entries_.end())
				throw std::runtime_error("Key not found");
			else
				return *it;
		}

		inline QJsonObject getAsJson(Key const& key) const
		{
			Value const& value = get(key);

			QJsonObject json;
			value.write(json);
			return json;
		}

		inline void remove(Key const& key)
		{
			entries_.remove(key);
		}

		inline typename MapType::key_value_iterator begin()
		{
			return entries_.keyValueBegin();
		}

		inline typename MapType::key_value_iterator end()
		{
			return entries_.keyValueEnd();
		}

	public:
		inline void read(const QJsonObject& obj) override
		{
			for (auto it = obj.begin(); it != obj.end(); ++it)
				addAsJson(it.key(), it.value().toObject());
		}

		inline void write(QJsonObject& obj) const override
		{
			for (auto it = entries_.begin(); it != entries_.end(); ++it)
			{
				QJsonObject json;
				it.value().write(json);

				obj[it.key()] = json;
			}
		}

	private:
		MapType entries_;
	};
}
