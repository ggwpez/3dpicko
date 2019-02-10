#include "include/apicontroller.h"
#include "include/colonydetection.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRandomGenerator>
#include <algorithm>
#include "httplistener.h"
#include "include/global.h"
#include "include/requestmapper.h"
#include "include/ws_server.hpp"
#include "include/command.h"
#include "include/octoprint.h"
#include "include/commands/arbitrary_command.h"
#include "include/gcodegenerator.h"

using namespace c3picko::pi;
using namespace c3picko::pi::commands;

static void OnStatusOk(int, Command::Response* response)
{
	if (response)
		qDebug() << "response: " << qPrintable(response->ToString());
}

static void OnStatusErr(QVariant status, Command::Response*) { qDebug() << qPrintable("Status error: " + status.toString()); }

static void OnNetworkErr(QString error) { qDebug() << qPrintable("Network error: " + error); }

static OctoPrint printer("192.168.2.2", "F866D6261972458CACAE9CB56E484758");

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
			emit OnImageDeleteError(req_data["id"].toString(), client);
			return;
		}
		// Is the image	in use by a job?
		for (auto pair : db_.jobs())
		{
			if (pair.second.img_id() == id)
			{
				emit OnImageDeleteError(req_data["id"].toString(), client);
				return;
			}
		}

		Image image = db_.images().get(id);
		image.clearCache();

		if (image.deleteFile())
		{
			db_.deletedImages().add(id, image);
			db_.images().remove(id); // Carefull here, if we use a reference to image instead, it will go out of scope after deletion
			emit OnImageDeleted(image, client);
		}
		else
		{
			emit OnImageDeleteError(image.path(), client);
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
	//		emit OnImageCreateError(,);
		}
		else if (! image.writeToFile())
		{
			emit OnImageCreateError(image.id(), client);
		}
		else
		{
			db_.images().add(image.id(), image);
			image.write(response);

			emit OnImageCreated(image, client);
		}
	}
	else if (path.startsWith("crop-image"))
	{
		QString img_id = req_data["id"].toString();
		int	x = req_data["x"].toDouble(), y = req_data["y"].toDouble(), w = req_data["width"].toDouble(),
			h = req_data["height"].toDouble();

		if (!db_.images().exists(img_id))
			return emit OnImageCropError(img_id, client);
		else
		{
			QString error;
			Image  cropped;
			Image& original = db_.images().get(img_id);

			// Is the cropped image empty?
			if (! original.crop(x, y, w, h, cropped, error))
			{
				emit OnImageCropError(img_id, client); // TODO inform client
				return;
			}
			if (! cropped.writeToFile())	// save cropped image to the hdd
			{
			//	emit OnImageCreateError();
				return;
			}

			db_.images().add(cropped.id(), cropped);
			response = {{"id", cropped.id()}}; // TODO
			emit OnImageCropped(cropped, client);
		}
	}
	else if (path.startsWith("createsettingsprofile"))
	{
		QJsonObject json_profile = request["data"].toObject();
		Profile::ID newId			  = db_.newProfileId();

		json_profile["id"] = newId; // TODO  hack
		Profile profile(json_profile);

		db_.profiles().add(newId, profile);
		profile.write(response);
		emit OnProfileCreated(newId, client);
	}
	else if (path.startsWith("updatesettingsprofile"))
	{
		QJsonObject json_profile = request["data"].toObject();
		Profile profile(json_profile);

		if (! db_.profiles().exists(profile.id()))
		{
			response = {{"error", "Profile Id unknown: '" +profile.id() +"'"}};
	//		emit OnProfileUpdateError();
		}
		else
		{
			db_.profiles().add(profile.id(), profile);
			response = json_profile;
			emit OnProfileUpdated(profile.id(), client);
		}
	}
	else if (path.startsWith("deletesettingsprofile"))
	{
		Profile::ID id = req_data["id"].toString();

		if (! db_.profiles().exists(id))
		{
		//	emit OnProfileDeleteError(id, client);
		}
		else
		{
			// FIXME cant delete profiles used by jobs
			db_.profiles().remove(id);
			response = {{"id", id}};
	//		emit OnProfileDeleteError();
		}
	}
	else if (path.startsWith("createjob"))
	{
		Job::ID jid = db_.newJobId();

		req_data["id"] = jid;
		Job new_job(req_data);			// FIXME validate profile ids
		new_job.resetCreationDate();	// cheat

		db_.jobs().add(jid, new_job);
		new_job.write(response);
		qDebug("%s", QString::fromUtf8(QJsonDocument(req_data).toJson()).toLatin1().constData());

		emit OnJobCreated(new_job, client);
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
			coords = Conversion::createColonyCoordinates(coords, data.cols,data.rows);
			QJsonArray json_coords;
			qDebug() << coords.size() << "Colonies";
			for (int i = 0; i < coords.size(); ++i)
			{
				json_coords.push_back(QJsonArray({coords[i][0], coords[i][1], coords[i][2]}));
			}
			response = {{"id", img_id}, {"coords", json_coords}};
		}
	}
	else if (path.startsWith("startjob"))
	{
		PrinterProfile* printerp = (PrinterProfile*)(db_.profiles().get("302"));
		PlateSocketProfile* socket = (PlateSocketProfile*)(db_.profiles().get("303"));
		PlateProfile* plate = (PlateProfile*)(db_.profiles().get("305"));

		GcodeGenerator gen(*socket, *printerp, *plate);

		std::vector<LocalColonyCoordinates> coords;
		for (int x = 0; x < 4; ++x)
			for (int y = 0; y < 5; ++y)
				coords.push_back(Point(10*x +70, y*10 +20));

		auto code = gen.CreateGcodeForTheEntirePickingProcess(1,8, coords);

		QStringList sum;
		for (auto c : code)
			sum << QString::fromStdString(c.ToString());

		Command*  cmd = ArbitraryCommand::MultiCommand(sum);

		QObject::connect(cmd, &Command::OnStatusOk, &OnStatusOk);
		QObject::connect(cmd, &Command::OnStatusErr, &OnStatusErr);
		QObject::connect(cmd, &Command::OnNetworkErr, &OnNetworkErr);

		QObject::connect(cmd, SIGNAL(OnFinished()), cmd, SLOT(deleteLater()));

		printer.SendCommand(cmd);
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
	else if (path.startsWith("backup"))
	{
		db_.saveToFile();
	}
	else
	{
		response = {{"error", "unknown request"}};
	}
}
} // namespace c3picko
