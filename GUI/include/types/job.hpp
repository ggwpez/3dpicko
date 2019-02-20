#pragma once

#include "include/json_constructable.hpp"
#include "include/types/image.hpp"
#include "include/types/profile.hpp"
#include <QDateTime>
#include <QString>

namespace c3picko
{
class Job : public JsonConstructable
{
  public:
	typedef QString ID;
	Job() = default;
	Job(QJsonObject const&);

  public:
	Image::ID img_id() const;
	ID		  id() const;
	void setId(ID id);
	void setCreationDate(QDateTime);

	void write(QJsonObject&) const override;

	Profile::ID socket() const;
	Profile::ID printer() const;
	Profile::ID plate() const;

  private:
	ID			id_;
	Image::ID   img_id_;
	QString		name_, description_;
	QDateTime   job_created_;
	Profile::ID plate_, printer_, socket_;

	/**
	 * @brief How far is the configuration of this Job?
	 */
	qint32 step_ = 0;
};
} // namespace c3picko
