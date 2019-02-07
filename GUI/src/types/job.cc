#include "include/types/job.hpp"

namespace c3picko
{
	Job::Job(const QJsonObject& obj)
		: JsonConstructable(obj),
		  id_(obj["id"].toString()),
		  img_id_(obj["img_id"].toString()),
		  name_(obj["name"].toString()),
		  description_(obj["description"].toString()),
		  job_created_(parseDateTime(obj["job_created"])),
		  step_(obj["step"].toInt())
	{

	}

	void Job::resetCreationDate()
	{
		job_created_ = QDateTime::currentDateTime();
	}

	Job::ID Job::id() const
	{
		return id_;
	}

	Image::ID Job::img_id() const
	{
		return img_id_;
	}

	void Job::write(QJsonObject& obj) const
	{
		obj["id"] = id_;
		obj["img_id"] = img_id_;
		obj["name"] = name_;
		obj["description"] = description_;
		obj["job_created"] = job_created_.toMSecsSinceEpoch();
		obj["step"] = step_;
	}
}
