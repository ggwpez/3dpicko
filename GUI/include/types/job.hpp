#pragma once

#include <QString>
#include <QDateTime>
#include <QTimeZone>
#include <QJsonObject>
#include "include/types/image.hpp"
#include "include/json_convertable.h"

namespace c3picko
{
	class Job : public JsonConvertable
	{
	public:
		typedef QString ID;
		Job() = default;
		Job(ID id, Image::ID img_id, QString name, QString description, QDateTime job_created, qint32 step);

	private:
		ID id_;
		Image::ID img_id_;
		QString name_, description;
		QDateTime job_created;

		/**
		 * @brief How far is the configuration of this Job?
		 */
		qint32 step = 0;

	public:
		Image::ID img_id() const;
		ID id() const;

		void read(const QJsonObject&) override;
		void write(QJsonObject&) const override;
	};
}
