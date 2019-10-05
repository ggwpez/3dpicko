#include "GUI/api_input.h"
#include "GUI/api_commands.h"
#include "GUI/api_controller.h"

namespace c3picko {
APIInput::APIInput(APIController* parent) : QObject(parent), api(parent) {}

void APIInput::serviceRequest(QJsonObject& request, QString const& raw_request,
							  QObject* client) {
  QString path = Marshalling::fromJson<QString>(request["request"]).toLower();
  QJsonObject req_data = request["data"].toObject();

  // All no-brainer requests are directly emittet as signals, indicating
  // success.
  // Complex request are parsed and forwarded to the api_controller.
  if (path == APICommands::GET_IMAGE_LIST) {
	emit api->OnImageListRequested(client);
  } else if (path == APICommands::GET_PROFILE_LIST) {
	emit api->OnProfileListRequested(client);
  } else if (path == APICommands::GET_VERSION_LIST) {
	emit api->OnVersionListRequested(client);
  } else if (path == APICommands::GET_JOB_LIST) {
	emit api->OnJobListRequested(client);
  } else if (path == APICommands::GET_DETECTION_ALGORITHMS) {
	emit api->OnAlgorithmListRequested(client);
  } else if (path == APICommands::SET_DEFAULT_SETTINGS_PROFILE) {
	Profile::ID id = Marshalling::fromJson<QString>(req_data["id"]);
	api->setDefaultSettingsProfile(id, client);
  } else if (path == APICommands::SET_STARTING_WELL) {
	Job::ID job_id = Marshalling::fromJson<QString>(req_data["job_id"]);
	Profile::ID plate_id = Marshalling::fromJson<QString>(req_data["plate_id"]);
	int row = req_data["row"].toInt();
	int col = req_data["column"].toInt();

	api->setStartingWell(job_id, plate_id, row, col, client);
  } else if (path == APICommands::DELETE_IMAGE) {
	Image::ID id = Marshalling::fromJson<QString>(req_data["id"]);

	api->DeleteImage(id, client);
  } else if (path == APICommands::DELETE_JOB) {
	QString id = Marshalling::fromJson<QString>(req_data["id"]);
	api->DeleteJob(id, client);
  } else if (path == APICommands::START_JOB) {
	Job::ID job = Marshalling::fromJson<QString>(req_data["id"]);
	Profile::ID octoprint =
		Marshalling::fromJson<Profile::ID>(req_data["octoprint_profile"]);

	api->startJob(job, client);
  } else if (path == APICommands::UPLOAD_IMAGE) {
	// Get image data
	QByteArray img_data(
		QByteArray::fromBase64(Marshalling::fromJson<QString>(req_data["file"])
								   .toUtf8()));  // TODO ugly code
	QString img_name =
		Marshalling::fromJson<QString>(req_data["original_filename"]);
	Image image =
		Image(img_data, img_name, "descrtiption", QDateTime::currentDateTime());

	api->UploadImage(std::move(image), client);
  } else if (path == APICommands::UPDATE_SOFTWARE) {
	api->updateSoftware(client);
  } else if (path == APICommands::CREATE_SETTINGS_PROFILE) {
	// Profile::ID newId		 = api->db().newProfileId();

	// json_profile["id"] = newId; // TODO  hack
	Profile profile_wo_id(Marshalling::fromJson<Profile>(req_data));

	api->createSettingsProfile(profile_wo_id, client);
  } else if (path == APICommands::UPDATE_SETTINGS_PROFILE) {
	Profile profile(Marshalling::fromJson<Profile>(req_data));

	api->updateSettingsProfile(profile, client);
  } else if (path == APICommands::DELETE_SETTINGS_PROFILE) {
	Profile::ID id = Marshalling::fromJson<QString>(req_data["id"]);

	api->deleteSettingsProfile(id, client);
  } else if (path == APICommands::CREATE_JOB) {
	Job job_wo_id(Marshalling::fromJson<Job>(req_data));

	api->createJob(job_wo_id, client);
  } else if (path == APICommands::SET_COLONIES_TO_PICK) {
	Job::ID job = Marshalling::fromJson<Job::ID>(req_data["job"]);
	quint32 number = req_data["number"].toInt();
	QSet<Colony::ID> ex_user = Marshalling::fromJson<QSet<Colony::ID>>(
						 req_data["ex_user"]),
					 in_user = Marshalling::fromJson<QSet<Colony::ID>>(
						 req_data["in_user"]);

	//
	// EXLUDED, INCLUDED
	// EX_USER, IN_USER
	// number
	//
	// (INCLUDED U IN_USER) \ EX_USER
	//

	api->setColoniesToPick(job, ex_user, in_user, number, client);
  } else if (path == APICommands::UPDATE_DETECTION_SETTINGS) {
	Job::ID job_id = Marshalling::fromJson<QString>(req_data["job_id"]);
	QString algo_id = Marshalling::fromJson<QString>(req_data["algorithm"]);
	QJsonObject settings = req_data["settings"].toObject();

	emit api->OnColonyDetectionStarted(job_id, client);
	api->updateDetectionSettings(job_id, algo_id, settings, client);
  } else if (path == APICommands::SHUTDOWN) {
	api->shutdown(client);
  } else if (path == APICommands::RESTART) {
	api->restart(client);
  } else if (path == APICommands::BACKUP) {
	api->db().saveToFile();
  } else {
	qWarning() << "Unknown request" << raw_request;
	emit api->OnUnknownRequest(path, client);
  }
}
}  // namespace c3picko
