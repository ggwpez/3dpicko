#include "include/database.hpp"
#include <QDebug>
#include <QJsonDocument>

namespace c3picko
{
Database::Database(QString file_path, QObject* parent) : QObject(parent), file_path_(file_path)
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

Database::JobTable& Database::jobTable() { return job_table_; }

Database::ImageTable& Database::imageTable() { return image_table_; }

Database::ImageTable& Database::deletedImageTable() { return deleted_image_table_; }

Database::JobTable& Database::deletedJobTable() { return deleted_job_table_; }

void Database::read(QJsonObject const& obj)
{
	job_table_.read(obj["jobs"].toObject());
	image_table_.read(obj["images"].toObject());
}

void Database::write(QJsonObject& obj) const
{
	obj["jobs"]			  = (QJsonObject)job_table_;
	obj["deleted_jobs"]   = (QJsonObject)deleted_job_table_;
	obj["images"]		  = (QJsonObject)image_table_;
	obj["deleted_images"] = (QJsonObject)deleted_image_table_;
}
}
