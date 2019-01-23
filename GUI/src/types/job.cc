#include "include/types/job.hpp"

namespace c3picko
{
	Job::Job(ID id, Image::ID img_id, QString name, QString description, QDateTime job_created, qint32 step)
		: id_(id), img_id_(img_id), name_(name), description(description), job_created(job_created), step(step)
	{

		}

		Job::ID Job::id() const
		{
			return id_;
		}

		Image::ID Job::img_id() const
		{
			return img_id_;
	}

	void Job::read(const QJsonObject& obj)
	{
		*this = Job(obj["id"].toString(), obj["img_id"].toString(), obj["name"].toString(), obj["description"].toString(), parseDateTime(obj["job_created"]), obj["step"].toInt());
	}

	void Job::write(QJsonObject& obj) const
	{
		obj["id"] = id_;
		obj["img_id"] = img_id_;
		obj["name"] = name_;
		obj["description"] = description;
		obj["job_created"] = job_created.toMSecsSinceEpoch();
		obj["step"] = step;
	}
}
