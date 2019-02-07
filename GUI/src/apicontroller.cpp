#include "include/apicontroller.h"
#include "include/colonydetection.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRandomGenerator>
#include <QMetaObject>

namespace c3picko
{

APIController::APIController(Database& db, QObject* parent) : QObject(parent), db_(db) {}

QJsonObject APIController::createImageList()
{
	QJsonArray image_list;
	for (auto const pair : db_.images())
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

	for (auto it = db_.jobs().begin(); it != db_.jobs().end(); ++it)
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

QJsonObject APIController::createProfileList()
{
	QJsonArray json_jobs;

	for (auto it = db_.profiles().begin(); it != db_.profiles().end(); ++it)
	{
		QJsonObject json;
		(*it).second.write(json);
		json_jobs.append(json);
	}

	return {{"profiles", json_jobs}};
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
	else if (path.startsWith("getprofilelist"))
	{
		response = createProfileList();
	}
	else if (path.startsWith("getjoblist"))
	{
		response = createJobList();
	}
	else if (path.startsWith("deleteimage"))
	{
		Image::ID id = req_data["id"].toString();
		// Does the image exist?
		if (!db_.images().exists(id))
		{
			emit OnFileDeleteError(req_data["id"].toString(), client);
			return;
		}
		// Is the image	in use by a job?
		for (auto pair : db_.jobs())
		{
			if (pair.second.img_id() == id)
			{
				emit OnFileDeleteError(req_data["id"].toString(), client);
				return;
			}
		}

		Image image = db_.images().get(id);
		image.clearCache();

		if (image.deleteFile())
		{
			db_.deletedImages().add(id, image);
			db_.images().remove(id); // Carefull here, if we use a reference to image instead, it will go out of scope after deletion
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
		if (!db_.jobs().exists(id))
		{
			emit OnJobDeleteError(req_data["id"].toString(), client);
		}
		else
		{
			Job job = db_.jobs().get(id);
			db_.jobs().remove(id);
			db_.deletedJobs().add(id, job);

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
		if (db_.images().exists(image.id()))
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

			db_.images().add(image.id(), image);
			image.write(response);

			emit OnNewFile(image, client);
		}
	}
	else if (path.startsWith("crop-image"))
	{
		QString img_id = req_data["id"].toString();
		int		x = req_data["x"].toDouble(), y = req_data["y"].toDouble(), w = req_data["width"].toDouble(),
			h = req_data["height"].toDouble();

		if (!db_.images().exists(img_id))
			return emit OnFileCropError(img_id, client);
		else
		{
			Image  cropped;
			Image& original = db_.images().get(img_id);
			if (x >= original.width() || y >= original.height() || std::min({w,h}) <= 0 || (x +w) >= original.width() || (y +h) >= original.height())
			{
				response = {{"error", "Invalid cropping area"}};
				qDebug() << x << y << w << h;
				return emit OnFileCropError(img_id, client);
			}

			// Is the cropped image empty?
			if (! original.crop(x, y, w, h, cropped))
			{
				qDebug() << "Cropping failed";
				emit OnFileCropError(img_id, client); // TODO inform client
				return;
			}
			cropped.writeToFile();	// save them to the hdd

			db_.images().add(cropped.id(), cropped);
			qDebug() << "Cropping image " << cropped.originalName();
			response = {{"id", cropped.id()}}; // TODO
			emit OnFileCropped(cropped, client);
		}
	}
	else if (path.startsWith("createsettingsprofile"))
	{
		QJsonObject json_profile = request["data"].toObject();
		// TODO create new profile
		// TODO create unique id
		ProfileWrapper::ID newId			  = db_.newProfileId();

		qDebug("%s", QString::fromUtf8(QJsonDocument(json_profile).toJson()).toLatin1().constData());
		json_profile["id"] = newId; // TODO  hack
		ProfileWrapper profile(json_profile);
		db_.profiles().add(newId, profile);
		qDebug() << "Added profile" << profile.id();
		profile.write(response);
	}
	else if (path.startsWith("updatesettingsprofile"))
	{
		qDebug() << "update lel";
		QJsonObject json_profile = request["data"].toObject();

		ProfileWrapper profile(json_profile);

		if (! db_.profiles().exists(profile.id()))
		{
			// TODO signal
			qWarning() << "Profile id unknown:" << profile.id();
			response = {{"error", "Profile Id unknown: '" +profile.id() +"'"}};
		}
		else
		{
			db_.profiles().add(profile.id(), profile);
			qDebug() << "Added profile" << profile.id();
			response = json_profile;
		}
	}
	else if (path.startsWith("deletesettingsprofile"))
	{
		ProfileWrapper::ID id = req_data["id"].toString();

		if (! db_.profiles().exists(id))
		{
			// TODO signal
			qWarning() << "Cant delete unknown profile id" << id;
		}
		else
		{
			// FIXME cant delete profiles used by jobs
			db_.profiles().remove(id);
			qDebug() << "Delete profile" << id;
			response = {{"id", id}};
		}
	}
	else if (path.startsWith("createjob"))
	{
		Job::ID jid = db_.newJobId();

		Job new_job(jid, req_data["img_id"].toString(), req_data["name"].toString(), req_data["description"].toString(),
					QDateTime::currentDateTime(), 2);
		db_.jobs().add(jid, new_job);
		new_job.write(response);

		emit OnNewJob(new_job, client);
	}
	else if (path.startsWith("getpositions"))
	{
		QString img_id = req_data["id"].toString();

		if (!db_.images().exists(img_id))
		{
			qWarning() << "Colony detection error: image not found (#" << img_id << ")";
		}
		else
		{
			Image& img = db_.images().get(img_id); // Non const& bc readCvMat() can set the cache
			cv::Mat data;

			if (!img.readCvMat(data))
			{
				qCritical() << "CV could not read image" << img.path();
				return;
			}

			std::vector<cv::Vec3f> coords = ColonyDetection::colonyDetection(data, "");
			coords = Conversion::createColonyCoordinates(coords, 900,600);
			QJsonArray json_coords;
			qDebug() << coords.size() << "Colonies";
			for (int i = 0; i < coords.size(); ++i)
			{
				json_coords.push_back(QJsonArray({coords[i][0], coords[i][1], coords[i][2]}));
			}
			response = {{"id", img_id}, {"coords", json_coords}};
		}
	}
	else if (path.startsWith("shutdown"))
	{
		qDebug() << "Client requested shutdown at" << QDateTime::currentDateTime().toString();
		QMetaObject::invokeMethod(qApp, "exit", Qt::QueuedConnection, Q_ARG(int, 0));
	}
	else if (path.startsWith("restart"))
	{
		qDebug() << "Client requested restart at" << QDateTime::currentDateTime().toString();
		QMetaObject::invokeMethod(qApp, "exit", Qt::QueuedConnection, Q_ARG(int, 123));
	}
	else
	{
		response = {{"error", "unknown request"}};
	}
}
} // namespace c3picko
