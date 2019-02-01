#include "include/apicontroller.h"
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QProcess>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QRandomGenerator>
#include "include/colonydetection.h"

namespace c3picko {

APIController::APIController(Database& db, QObject* parent)
	: QObject(parent), db_(db)
{

}

QJsonObject APIController::createImageList()
{
	QJsonArray image_list;
	for (auto const pair : db_.imageTable())
	{
		Image const& image = pair.second;
		QJsonObject json;

		image.write(json);
		image_list.push_back(json);
	}

	return {{ "images", image_list }};
}

QJsonObject APIController::createImageList(Image img)
{
	QJsonArray json_jobs;
	QJsonObject json_job;

	img.write(json_job);
	json_jobs.append(json_job);

	return {{ "images", json_jobs }};
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

	return {{ "jobs", json_jobs }};
}

QJsonObject APIController::createJobList(Job job)
{
	QJsonArray json_jobs;
	QJsonObject json_job;

	job.write(json_job);
	json_jobs.append(json_job);

	return {{ "jobs", json_jobs }};
}

QJsonObject APIController::createDeleteImage(Image img)
{
	return {{"id", img.id()}};
}

QJsonObject APIController::createDeleteJob(Job job)
{
	return {{"id", job.id()}};
}

void APIController::service(QJsonObject& request, QJsonObject& response, QObject* client)
{
	QString path = request["request"].toString().toLower();
	QJsonObject req_data = request["data"].toObject();

	if (path.startsWith("getimagelist"))
	{
		response = createImageList();
	}
	else if (path.startsWith("deleteimage"))
	{
		QString id = req_data["id"].toString();
		// Does the image exist?
		if (! db_.imageTable().exists(id))
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
		QDir file(DocRoot());

		if(file.remove(image.path().mid(1)))
		{
			db_.imageTable().remove(id);
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
		if (! db_.jobTable().exists(id))
		{
			emit OnJobDeleteError(req_data["id"].toString(), client);
			return;
		}

		Job job = db_.jobTable().get(id);
		db_.jobTable().remove(id);
		emit OnJobDeleted(job, client);
	}
	else if (path.startsWith("uploadimage"))
	{
		// Get image data
		QByteArray img_data(QByteArray::fromBase64(req_data["file"].toString().toUtf8()));	// TODO ugly code
		QString img_id = QCryptographicHash::hash(img_data, QCryptographicHash::Sha256).toHex();
		QString img_name = req_data["original_filename"].toString();
		QDateTime img_created = QDateTime::currentDateTime();

		// Is the user trying to upload an image twice?
		if (db_.imageTable().exists(img_id))
		{
			qDebug() << "Ignoring doubled image";
			// TODO inform client?
		}
		else
		{
			/*
			 * drag and drop image upload handler
			 * sends json object: {"id" : "id", "path" : "path_to_image", "filename" : "original_filename_without_extension", "upload_date" : "last_modified_date_string"}
			*/
			QTemporaryFile file(UploadFolder() +"XXXXXXXX");
			file.setAutoRemove(false);

			if (! file.open())
			{
				emit OnFileUploadError("<QTemporaryFile>", client);
			}
			else
			{
				QString img_path = '/' +UploadFolderName() +'/' +QFileInfo(file).fileName();
				Image image(img_id, img_name, "", img_path, img_created);
				file.write(img_data);
				file.close();

				db_.imageTable().add(img_id, image);
				image.write(response);

				emit OnNewFile(image, client);	// This informs the client of the new image
			}
		}
	}
	else if (path.startsWith("crop-image"))
	{
		QString img_id = request["id"].toString();
		QJsonObject window = request["window"].toObject();
		cv::Rect rect(window["x"].toInt(), window["y"].toInt(), window["width"].toInt(), window["height"].toInt());

		if (! db_.imageTable().exists(img_id))
		{
			emit OnFileCropError(img_id, client);
		}
		else
		{
			Image const& img = db_.imageTable().get(img_id);

			cv::Mat data = cv::imread((DocRoot() +img.path()).toLatin1().constData());
			cv::Mat cropped(data, rect);

			Image cropped_img(img);
		}
	}
	else if (path.startsWith("createsettingsprofile")){
		QJsonObject json_profile_object = request["data"].toObject();
		// TODO create new profile
		// TODO create unique id
		QString newId = QString::number(QRandomGenerator::global()->generate());
		json_profile_object["id"] = newId;
		response = json_profile_object;
	}
	else if (path.startsWith("updatesettingsprofile")){
		QJsonObject json_profile_object = request["data"].toObject();
		// TODO update profile
		response = json_profile_object;
	}
	else if (path.startsWith("createjob"))
	{
		static unsigned short id = 456;
		QString jid = QString::number(id);

		Job new_job(jid, req_data["img_id"].toString(), req_data["name"].toString(), req_data["description"].toString(), QDateTime::currentDateTime(), 2);
		db_.jobTable().add(jid, new_job);
		new_job.write(response);

		++id;
		emit OnNewJob(new_job, client);
	}
	else if (path.startsWith("getpositions"))
	{
		QString img_id = req_data["id"].toString();

		if (! db_.imageTable().exists(img_id))
		{
			qWarning() << "Colony detection error: image not found (#" << img_id << ")";
		}
		else
		{
			Image const& img = db_.imageTable().get(img_id);

			cv::Mat data = cv::imread((DocRoot() +img.path()).toLatin1().constData(), CV_LOAD_IMAGE_COLOR);

			if (! data.data)
			{
				qCritical() << "CV could not read image" << img.path();
				return;
			}

			std::vector<cv::Vec3f> coords = ColonyDetection::colonyDetection(data, "");
			QJsonArray json_coords;
			qDebug() << coords.size() << "Colonies";
			for (int i = 0; i < coords.size(); ++i)
			{
				json_coords.push_back(QJsonArray({ coords[i][0], coords[i][1], coords[i][2] }));
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
		 response = {{ "error", "unknown request" }};
	}
}
} // namespace c3picko
