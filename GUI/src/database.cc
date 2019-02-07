#include "include/database.hpp"
#include <QDebug>
#include <QJsonDocument>

namespace c3picko
{
Database::Database(QString file_path, QObject* parent) : QObject(parent), file_path_(file_path),
	image_id_(100), job_id_(200), profile_id_(300)
{
	QFile file(file_path);

	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray  data = file.readAll();
		QJsonObject json = QJsonDocument::fromJson(data).object();
		read(json);

		qDebug() << "Loaded database form file" << file_path_;
	}
	else
		qCritical() << "Error reading database from file" << file_path_;
}

Database::~Database() { saveToFile(); }

void Database::saveToFile()
{
	QFile file(file_path_);

	if (file.open(QIODevice::WriteOnly))
	{
		QJsonObject json;
		write(json);
		QByteArray data = QJsonDocument(json).toJson();

		file.write(data);
		qDebug() << "Saving database to file" << file_path_;
	}
	else
		qCritical() << "Error saving database to file" << file_path_;
}

Database::JobTable& Database::jobs() { return jobs_; }

Database::ImageTable& Database::images() { return images_; }

Database::ImageTable& Database::deletedImages() { return deleted_images_; }

Database::ProfileTable&Database::profiles()
{
	return profiles_;
}

Image::ID Database::newImageId()
{
	return QString::number(image_id_++);
}

Job::ID Database::newJobId()
{
	return QString::number(job_id_++);
}

ProfileWrapper::ID Database::newProfileId()
{
	return QString::number(profile_id_++);
}

Database::JobTable& Database::deletedJobs() { return deleted_jobs_; }

void Database::read(QJsonObject const& obj)
{
	jobs_.read(obj["jobs"].toObject());
	deleted_jobs_.read(obj["deleted_jobs"].toObject());
	images_.read(obj["images"].toObject());
	deleted_images_.read(obj["deleted_images"].toObject());

	profiles_.read(obj["profiles"].toObject());

	image_id_ = obj["image_id"].toInt();
	job_id_ = obj["job_id"].toInt();
	profile_id_ = obj["profile_id"].toInt();
}

void Database::write(QJsonObject& obj) const
{
	obj["jobs"]			  = (QJsonObject)jobs_;
	obj["deleted_jobs"]   = (QJsonObject)deleted_jobs_;
	obj["images"]		  = (QJsonObject)images_;
	obj["deleted_images"] = (QJsonObject)deleted_images_;

	obj["profiles"] = (QJsonObject)profiles_;
}
}
