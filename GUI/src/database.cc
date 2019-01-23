#include "include/database.hpp"
#include <QJsonDocument>
#include <QDebug>

namespace c3picko {

	Database::Database(QString file_path, QObject* parent)
		: QObject(parent), file_path_(file_path)
	{
		QFile file(file_path);

		if (file.open(QIODevice::ReadOnly))
		{
			QByteArray data = file.readAll();
			QJsonObject json = QJsonDocument::fromJson(data).object();
			read(json);


		}
		else
			qCritical() << "Error reading database from file" << file_path_;
	}

	Database::~Database()
	{
		saveToFile();
	}

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

	Database::JobTable&Database::jobTable()
	{
		return job_table_;
	}

	Database::ImageTable&Database::imageTable()
	{
		return image_table_;
	}

	void Database::read(const QJsonObject& obj)
	{
		job_table_.read(obj["jobs"].toObject());
		image_table_.read(obj["images"].toObject());
	}

	void Database::write(QJsonObject& obj) const
	{
		QJsonObject jobs, images;

		job_table_.write(jobs);
		image_table_.write(images);

		obj["jobs"] = jobs;
		obj["images"] = images;
	}
}
