#include "include/api_input.h"
#include "include/api_controller.h"

namespace c3picko
{

APIInput::APIInput(APIController* parent) : QObject(parent), api(parent) {}

void APIInput::serviceRequest(QJsonObject& request, QString const& raw_request, QObject* client)
{
	QString		path	 = request["request"].toString().toLower();
	QJsonObject req_data = request["data"].toObject();

	// All no-brainer requests are directly emittet as signals, indicating
	// success.
	// Complex request are parsed and forwarded to the api_controller.
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

		api->DeleteImage(id, client);
	}
	else if (path == "deletejob")
	{
		QString id = req_data["id"].toString();
		api->DeleteJob(id, client);
	}
	else if (path == "uploadimage")
	{
		// Get image data
		QByteArray img_data(QByteArray::fromBase64(req_data["file"].toString().toUtf8())); // TODO ugly code
		QString	img_name = req_data["original_filename"].toString();
		Image	  image	= Image(img_data, img_name, "descrtiption", QDateTime::currentDateTime());
		qDebug() << "Hash" << image.id();

		api->UploadImage(image, client);
	}
	else if (path == "crop-image")
	{
		QString img_id = req_data["id"].toString();
		int		x = req_data["x"].toDouble(), y = req_data["y"].toDouble(), w = req_data["width"].toDouble(),
			h = req_data["height"].toDouble();

		api->cropImage(img_id, x, y, w, h, client);
	}
	else if (path == "createsettingsprofile")
	{
		QJsonObject json_profile = req_data;
		// Profile::ID newId		 = api->db().newProfileId();

		// json_profile["id"] = newId; // TODO  hack
		Profile profile_wo_id(json_profile);

		api->createSettingsProfile(profile_wo_id, client);
	}
	else if (path == "updatesettingsprofile")
	{
		QJsonObject json_profile = request["data"].toObject();
		Profile		profile(json_profile);

		api->updateSettingsProfile(profile, client);
	}
	else if (path == "deletesettingsprofile")
	{
		Profile::ID id = req_data["id"].toString();

		api->deleteSettingsProfile(id, client);
	}
	else if (path == "createjob")
	{
		Job job_wo_id(req_data);

		api->createJob(job_wo_id, client);
	}
	else if (path == "getpositions")
	{
		QString img_id = req_data["id"].toString();
		api->getPositions(img_id, client);
	}
	else if (path == "updatedetectionsettings")
	{
		Job::ID		job_id   = req_data["job_id"].toString();
		QString		algo_id  = req_data["algorithm"].toString();
		QJsonObject settings = req_data["settings"].toObject();

		emit api->OnColonyDetectionStarted(job_id, client);
		api->updateDetectionSettings(job_id, algo_id, settings, client);
	}
	else if (path == "startjob")
	{
		api->startJob("", client);
	}
	else if (path == "shutdown")
	{
		api->shutdown(client);
	}
	else if (path == "restart")
	{
		api->restart(client);
	}
	else if (path == "backup")
	{
		api->db().saveToFile();
	}
	else
	{
		qWarning() << "Unknown request" << raw_request;
		emit api->OnUnknownRequest("", client);
		// response = {{"error", "unknown request"}};
	}
}
} // namespace c3picko
