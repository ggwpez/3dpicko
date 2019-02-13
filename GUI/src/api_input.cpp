#include "include/api_input.h"
#include "include/colonydetector.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QThreadPool>
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
#include "include/api_controller.h"

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

APIInput::APIInput(APIController* parent)
	: QObject(parent), api(parent)
{

}

QJsonObject APIInput::createImageList()
{
	QJsonArray image_list;
	for (auto const& image : api->db().images())
	{
		QJsonObject  json;

		image.write(json);
		image_list.push_back(json);
	}

	return {{"images", image_list}};
}

QJsonObject APIInput::createImageList(Image img)
{
	QJsonArray  json_jobs;
	QJsonObject json_job;

	img.write(json_job);
	json_jobs.append(json_job);

	return {{"images", json_jobs}};
}

QJsonObject APIInput::createJobList()
{
	QJsonArray json_jobs;

	for (auto const& job : api->db().jobs())
	{
		QJsonObject json;
		job.write(json);
		json_jobs.append(json);
	}

	return {{"jobs", json_jobs}};
}

QJsonObject APIInput::createJobList(Job job)
{
	QJsonArray  json_jobs;
	QJsonObject json_job;

	job.write(json_job);
	json_jobs.append(json_job);

	return {{"jobs", json_jobs}};
}

QJsonObject APIInput::createProfileList()
{
	QJsonArray json_jobs;

	for (auto const& profile : api->db().profiles())
	{
		QJsonObject json;
		profile.write(json);
		json_jobs.append(json);
	}

	return {{"profiles", json_jobs}};
}

QJsonObject APIInput::createDeleteImage(Image img) { return {{"id", img.id()}}; }

QJsonObject APIInput::createDeleteJob(Job job) { return {{"id", job.id()}}; }

void APIInput::service(QJsonObject& request, QObject* client)
{
	QString		path	 = request["request"].toString().toLower();
	QJsonObject req_data = request["data"].toObject();

	if (path == "getimagelist")
	{
		emit api->OnImageListRequested(client);
	}
	else if (path == "getprofilelist")
	{
		emit api->OnProfileListRequested(client);
	}
	else if (path == "getjoblist")
	{
		emit api->OnJobListRequested(client);
	}
	else if (path == "getdetectionalgorithms")
	{
		emit api->OnAlgorithmListRequested(client);
	}
	else if (path == "deleteimage")
	{
		Image::ID id = req_data["id"].toString();
		// Does the image exist?
		if (!api->db().images().exists(id))
		{
			emit api->OnImageDeleteError(req_data["id"].toString(), client);
			return;
		}
		// Is the image	used by a job?
		for (auto const& job : api->db().jobs())
		{
			if (job.img_id() == id)
			{
				emit api->OnImageDeleteError(req_data["id"].toString(), client);
				return;
			}
		}

		Image image = api->db().images().get(id);
		image.clearCache();

		if (image.deleteFile())
		{
			api->db().deletedImages().add(id, image);
			api->db().images().remove(id); // Carefull here, if we use a reference to image instead, it will go out of scope after deletion
			emit api->OnImageDeleted(image, client);
		}
		else
		{
			emit api->OnImageDeleteError(image.path(), client);
		}
	}
	else if (path == "deletejob")
	{
		QString id = req_data["id"].toString();
		// Does the job exist?
		if (!api->db().jobs().exists(id))
		{
			emit api->OnJobDeleteError(req_data["id"].toString(), client);
		}
		else
		{
			Job job = api->db().jobs().get(id);
			api->db().jobs().remove(id);
			api->db().deletedJobs().add(id, job);

			emit api->OnJobDeleted(job, client);
		}
	}
	else if (path == "uploadimage")
	{
		// Get image data
		QByteArray img_data(QByteArray::fromBase64(req_data["file"].toString().toUtf8())); // TODO ugly code
		QString	img_name = req_data["original_filename"].toString();
		Image	  image	= Image(img_data, img_name, "descrtiption", QDateTime::currentDateTime());
		qDebug() << "Hash" << image.id();

		// Is the user trying to upload an image twice?
		if (api->db().images().exists(image.id()))
		{
			qDebug() << "Ignoring doubled image";
			// TODO inform client?
			emit api->OnImageCreateError("<cropped>", client);
		}
		else if (! image.writeToFile())
		{
			emit api->OnImageCreateError(image.id(), client);
		}
		else
		{
			api->db().images().add(image.id(), image);
			//image.write(response);

			emit api->OnImageCreated(image, client);
		}
	}
	else if (path == "crop-image")
	{
		QString img_id = req_data["id"].toString();
		int	x = req_data["x"].toDouble(), y = req_data["y"].toDouble(), w = req_data["width"].toDouble(),
			h = req_data["height"].toDouble();

		if (!api->db().images().exists(img_id))
			return emit api->OnImageCropError(img_id, client);
		else
		{
			QString error;
			Image  cropped;
			Image& original = api->db().images().get(img_id);

			// Is the cropped image empty?
			if (! original.crop(x, y, w, h, cropped, error))
			{
				emit api->OnImageCropError(img_id, client); // TODO inform client
				return;
			}
			if (! cropped.writeToFile())	// save cropped image to the hdd
			{
				emit api->OnImageCreateError("<cropped>", client);
				return;
			}

			api->db().images().add(cropped.id(), cropped);
			//response = {{"id", cropped.id()}}; // TODO
			emit api->OnImageCropped(cropped, client);
		}
	}
	else if (path == "createsettingsprofile")
	{
		QJsonObject json_profile = request["data"].toObject();
		Profile::ID newId			  = api->db().newProfileId();

		json_profile["id"] = newId; // TODO  hack
		Profile profile(json_profile);

		api->db().profiles().add(newId, profile);
		//profile.write(response);
		emit api->OnProfileCreated(newId, client);
	}
	else if (path == "updatesettingsprofile")
	{
		QJsonObject json_profile = request["data"].toObject();
		Profile profile(json_profile);

		if (! api->db().profiles().exists(profile.id()))
		{
			//response = {{"error", "Profile Id unknown: '" +profile.id() +"'"}};
			emit api->OnProfileUpdateError(profile.id(), client);
		}
		else
		{
			api->db().profiles().add(profile.id(), profile);
			//response = json_profile;
			emit api->OnProfileUpdated(profile.id(), client);
		}
	}
	else if (path == "deletesettingsprofile")
	{
		Profile::ID id = req_data["id"].toString();

		if (! api->db().profiles().exists(id))
		{
			emit api->OnProfileDeleteError(id, client);
		}
		else
		{
			// FIXME cant delete profiles used by jobs
			emit api->OnProfileDeleted(api->db().profiles().get(id), client);
			//response = {{"id", id}};
			api->db().profiles().remove(id);
		}
	}
	else if (path == "createjob")
	{
		Job::ID jid = api->db().newJobId();

		req_data["id"] = jid;
		Job new_job(req_data);			// FIXME validate profile ids
		new_job.resetCreationDate();	// cheat

		api->db().jobs().add(jid, new_job);
		//new_job.write(response);
		qDebug("%s", QString::fromUtf8(QJsonDocument(req_data).toJson()).toLatin1().constData());

		emit api->OnJobCreated(new_job, client);
	}
	else if (path == "getpositions")
	{
		QString img_id = req_data["id"].toString();

		if (!api->db().images().exists(img_id))
		{
			qWarning() << "Colony detection error: image not found (#" << img_id << ")";
			emit api->OnColonyDetectionError("Image not found", client);
		}
		else
		{
			Image& img = api->db().images().get(img_id); // Non const& bc readCvMat() can set the cache
			cv::Mat data;

			if (!img.readCvMat(data))
			{
				qCritical() << "CV could not read image" << img.path();
				emit api->OnColonyDetectionError("Image not readable or empty", client);
				return;
			}

			ColonyDetector* detector = new ColonyDetector(data.clone());	// Clone to avoid thread problems TODO debug
			// Set the connection context to detector, so in case it timeouts it dosent leak memory
			connect(detector, &ColonyDetector::OnFinished, detector, [this](qint64 ms)
			{
				qDebug() << "Colony detected in" << ms << "ms";
				emit api->OnColonyDetected();
			});
			detector->setAutoDelete(true);
			api->pool()->start(detector);

			/*std::vector<cv::Vec3f> coords = ColonyDetector::colonyDetection(data, "");
			coords = Conversion::createColonyCoordinates(coords, data.cols,data.rows);
			QJsonArray json_coords;
			qDebug() << coords.size() << "Colonies";
			for (int i = 0; i < coords.size(); ++i)
			{
				json_coords.push_back(QJsonArray({coords[i][0], coords[i][1], coords[i][2]}));
			}
			response = {{"id", img_id}, {"coords", json_coords}};*/
		}
	}
	else if (path == "startjob")
	{
		PrinterProfile* printerp = (PrinterProfile*)(api->db().profiles().get("302"));
		PlateSocketProfile* socket = (PlateSocketProfile*)(api->db().profiles().get("303"));
		PlateProfile* plate = (PlateProfile*)(api->db().profiles().get("305"));

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
	else if (path == "shutdown")
	{
		qDebug() << "Client requested shutdown at" << QDateTime::currentDateTime().toString();
		qApp->exit(0);
	}
	else if (path == "restart")
	{
		qDebug() << "Client requested restart at" << QDateTime::currentDateTime().toString();
		qApp->exit(123);
	}
	else if (path == "backup")
	{
		api->db().saveToFile();
	}
	else
	{
		//response = {{"error", "unknown request"}};
	}
}
} // namespace c3picko
