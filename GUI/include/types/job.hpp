#pragma once

#include <QString>
#include <QDateTime>
#include <QTimeZone>
#include <QJsonObject>
#include "include/types/image.hpp"
#include "include/json_constructable.hpp"

namespace c3picko
{
	class Job : public JsonConstructable
	{
	public:
		typedef QString ID;
		Job() = default;
		Job(QJsonObject const&);
		Job(ID id, Image::ID img_id, QString name, QString description, QDateTime job_created, qint32 step);

	private:
		ID id_;
		Image::ID img_id_;
		QString name_, description_;
		QDateTime job_created_;

		/**
		 * @brief How far is the configuration of this Job?
		 */
		qint32 step_ = 0;

	public:
		Image::ID img_id() const;
		ID id() const;

		void write(QJsonObject&) const override;
	};
}
