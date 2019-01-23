#pragma once

#include "include/json_convertable.h"
#include <QDateTime>

namespace c3picko {
	class Image : public JsonConvertable
	{
	public:
		typedef QString ID;
		Image() = default;
		Image(ID id, QString original_name, QString description, QString path, QDateTime uploaded);

	private:
		ID id_;
		QString original_name_;
		QString description_;
		// TODO we can add meta data here
		QString path_;
		QDateTime uploaded_;

	public:
		QString path() const;
		ID id() const;

		void read(const QJsonObject&) override;
		void write(QJsonObject&) const override;
	};
}
