#include "include/apicontroller.h"
#include "include/colonydetection.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRandomGenerator>

namespace c3picko
{

APIController::APIController(Database& db, QObject* parent) : QObject(parent), db_(db) {}

QJsonObject APIController::createImageList()
{
	QJsonArray image_list;
	for (auto const pair : db_.imageTable())
	{
		Image const& image = pair.second;
		QJsonObject  json;

		image.write(json);
		image_list.push_back(json);
	}

	return {{"images", image_list}};
}

QJsonObject APIController::createImageList(Image img)
{
	QJsonArray  json_jobs;
	QJsonObject json_job;

	img.write(json_job);
	json_jobs.append(json_job);

	return {{"images", json_jobs}};
}

QJsonObject APIController::createJobList()
{
	QJsonArray json_jobs;

	for (auto it = db_.jobTable().begin(); it != db_.jobTable().end(); ++it)
	{
		QJsonObject json;
		(*it).second.write(json);
		json_jobs.append(json);
	}

	return {{"jobs", json_jobs}};
}

QJsonObject APIController::createJobList(Job job)
{
	QJsonArray  json_jobs;
	QJsonObject json_job;

	job.write(json_job);
	json_jobs.append(json_job);

	return {{"jobs", json_jobs}};
}

QJsonObject APIController::createDeleteImage(Image img) { return {{"id", img.id()}}; }

QJsonObject APIController::createDeleteJob(Job job) { return {{"id", job.id()}}; }

void APIController::service(QJsonObject& request, QJsonObject& response, QObject* client)
{
	QString		path	 = request["request"].toString().toLower();
	QJsonObject req_data = request["data"].toObject();

	if (path.startsWith("getimagelist"))
	{
		response = createImageList();
	}
	else if (path.startsWith("deleteimage"))
	{
		Image::ID id = req_data["id"].toString();
		// Does the image exist?
		if (!db_.imageTable().exists(id))
		{
			emit OnFileDeleteError(req_data["id"].toString(), client);
			return;
		}
		// Is the image	in use by a job?
		for (auto pair : db_.jobTable())
		{
			if (pair.second.img_id() == id)
			{
				emit OnFileDeleteError(req_data["id"].toString(), client);
				return;
			}
		}

		Image image = db_.imageTable().get(id);
		image.clearCache();

		if (image.deleteFile())
		{
			db_.deletedImageTable().add(id, image);
			db_.imageTable().remove(id); // Carefull here, if we use a reference to image instead, it will go out of scope after deletion
			emit OnFileDeleted(image, client);
		}
		else
		{
			emit OnFileDeleteError(image.path(), client);
		}
	}
	else if (path.startsWith("deletejob"))
	{
		QString id = req_data["id"].toString();
		// Does the job exist?
		if (!db_.jobTable().exists(id))
		{
			emit OnJobDeleteError(req_data["id"].toString(), client);
		}
		else
		{
			Job job = db_.jobTable().get(id);
			db_.jobTable().remove(id);
			db_.deletedJobTable().add(id, job);

			emit OnJobDeleted(job, client);
		}
	}
	else if (path.startsWith("uploadimage"))
	{
		// Get image data
		QByteArray img_data(QByteArray::fromBase64(req_data["file"].toString().toUtf8())); // TODO ugly code
		QString	img_name = req_data["original_filename"].toString();
		Image	  image	= Image(img_data, img_name, "descrtiption", QDateTime::currentDateTime());
		qDebug() << "Hash" << image.id();

		// Is the user trying to upload an image twice?
		if (db_.imageTable().exists(image.id()))
		{
			qDebug() << "Ignoring doubled image";
			// TODO inform client?
		}
		else
		{
			/*
			 * drag and drop image upload handler
			 * sends json object: {"id" : "id", "path" : "path_to_image", "filename" : "original_filename_without_extension", "upload_date"
			 * : "last_modified_date_string"}
			*/
			if (!image.writeToFile())
			{
				emit OnFileUploadError(image.id(), client);
				return;
			}

			db_.imageTable().add(image.id(), image);
			image.write(response);

			emit OnNewFile(image, client);
		}
	}
	else if (path.startsWith("crop-image"))
	{
		QString img_id = req_data["id"].toString();
		int		x = req_data["x"].toDouble(), y = req_data["y"].toDouble(), w = req_data["width"].toDouble(),
			h = req_data["height"].toDouble();

		if (std::min(x, y) < 100 || std::min(w, h) < 100)
		{
			response = {{"error", "Invalid cropping area"}};
			qDebug() << x << y << w << h;
			return emit OnFileCropError(img_id, client);
		}

		if (!db_.imageTable().exists(img_id))
			return emit OnFileCropError(img_id, client);
		else
		{
			Image& original = db_.imageTable().get(img_id);
			Image  cropped  = original.crop(x, y, w, h);

			// Is the cropped image empty?
			if (!cropped.width() || cropped.height())
			{
				qDebug() << "Cropping failed";
				emit OnFileCropError(img_id, client); // TODO inform client
				return;
			}

			db_.imageTable().add(cropped.id(), cropped);
			qDebug() << "Cropping image " << cropped.originalName();
			response = {{"id", cropped.id()}}; // TODO
			emit OnFileCropped(cropped, client);
		}
	}
	else if (path.startsWith("createsettingsprofile"))
	{
		QJsonObject json_profile_object = request["data"].toObject();
		// TODO create new profile
		// TODO create unique id
		QString newId			  = QString::number(QRandomGenerator::global()->generate());
		json_profile_object["id"] = newId;
		response				  = json_profile_object;
	}
	else if (path.startsWith("updatesettingsprofile"))
	{
		QJsonObject json_profile_object = request["data"].toObject();
		// TODO update profile
		response = json_profile_object;
	}
	else if (path.startsWith("createjob"))
	{
		static unsigned short id  = 456;
		QString				  jid = QString::number(id);

		Job new_job(jid, req_data["img_id"].toString(), req_data["name"].toString(), req_data["description"].toString(),
					QDateTime::currentDateTime(), 2);
		db_.jobTable().add(jid, new_job);
		new_job.write(response);

		++id;
		emit OnNewJob(new_job, client);
	}
	else if (path.startsWith("getpositions"))
	{
		QString img_id = req_data["id"].toString();

		if (!db_.imageTable().exists(img_id))
		{
			qWarning() << "Colony detection error: image not found (#" << img_id << ")";
		}
		else
		{
			Image& img = db_.imageTable().get(img_id); // Non const& bc readCvMat() can set the cache

			cv::Mat data = *img.readCvMat();

			if (!data.data)
			{
				qCritical() << "CV could not read image" << img.path();
				return;
			}

			std::vector<cv::Vec3f> coords = ColonyDetection::colonyDetection(data, "");
			QJsonArray			   json_coords;
			qDebug() << coords.size() << "Colonies";
			for (int i = 0; i < coords.size(); ++i)
			{
				json_coords.push_back(QJsonArray({coords[i][0], coords[i][1], coords[i][2]}));
			}
			response = {{"id", img_id}, {"coords", json_coords}};
		}
	}
	else if (path.startsWith("getjoblist"))
	{
		response = createJobList();
	}
	else if (path.startsWith("shutdown"))
	{
		qDebug() << "Client requested shutdown at" << QDateTime::currentDateTime().toString();
		qApp->exit(0);
	}
	else if (path.startsWith("restart"))
	{
		qDebug() << "Client requested restart at" << QDateTime::currentDateTime().toString();
		qApp->exit(123);
	}
	else
	{
		response = {{"error", "unknown request"}};
	}
}
} // namespace c3picko
