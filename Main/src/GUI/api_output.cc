#include "GUI/api_output.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QVariant>
#include "GUI/api_commands.h"
#include "GUI/api_controller.h"
#include "ImageRecognition/colony.h"

namespace c3picko {
APIOutput::APIOutput(APIController* parent)
	: QObject((QObject*)parent), op(parent) {}

void APIOutput::UnknownRequest(QString request, QObject* client) {
  Error("Unknown request", request, client);
}

void APIOutput::VersionListRequested(QObject* client) {
  emit op->toClient(client, APICommands::GET_VERSION_LIST,
					op->createVersionList());
}

void APIOutput::ImageListRequested(QObject* client) {
  emit op->toClient(client, APICommands::GET_IMAGE_LIST, op->createImageList());
}

void APIOutput::JobListRequested(QObject* client) {
  emit op->toClient(client, APICommands::GET_JOB_LIST, op->createJobList());
}

void APIOutput::ProfileListRequested(QObject* client) {
  emit op->toClient(client, APICommands::GET_PROFILE_LIST,
					op->createProfileList());
}

void APIOutput::AlgorithmListRequested(QObject* client) {
  emit op->toClient(client, APICommands::GET_DETECTION_ALGORITHMS,
					op->CreateAlgorithmList());
}

void APIOutput::JobCreated(Job::ID id, QObject* client) {
  Job const& job(op->db().jobs().get(id));
  QJsonObject json(Marshalling::toJson(job));

  emit op->toClient(client, APICommands::CREATE_JOB, json);
  emit op->toAllExClient(client, APICommands::GET_JOB_LIST, json);  // TODO lazy
}

void APIOutput::JobCreateError(QString id, QObject* client) {
  Error(APICommands::CREATE_JOB, id, client);
}

void APIOutput::JobDeleted(Job::ID id, QObject*) {
  emit op->toAll(APICommands::DELETE_JOB, {{"id", id}});
}

void APIOutput::JobDeleteError(Job::ID id, QObject* client) {
  Error(APICommands::DELETE_JOB, "id=" + id, client);
}

void APIOutput::JobStarted(Job::ID id, QObject*) {
  emit op->toAll(APICommands::START_JOB, {{"id", id}});
}

void APIOutput::JobStartError(QString error, QObject* client) {
  Error(APICommands::START_JOB, error, client);
}

void APIOutput::Report(Job::ID id, QString path, QObject* client) {
  emit op->toAll(APICommands::GET_REPORT, {{"id", id}, {"path", path}});
}

void APIOutput::ReportError(QString error, QObject* client) {
  Error(APICommands::GET_REPORT, "error=" + error, client);
}

void APIOutput::ImageCreated(Image::ID id, QObject*) {
  Image const& image(op->db().images().get(id));
  QJsonObject json(Marshalling::toJson(image));

  emit op->toAll(APICommands::UPLOAD_IMAGE, json);
}

void APIOutput::ImageCreateError(QString path, QObject* client) {
  Error(APICommands::UPLOAD_IMAGE, "path=" + path, client);
}

void APIOutput::ImageDeleted(Image::ID id, QObject*) {
  emit op->toAll(APICommands::DELETE_IMAGE, {{"id", id}});
}

void APIOutput::ImageDeleteError(QString path, QObject* client) {
  Error(APICommands::DELETE_IMAGE, "path=" + path, client);
}

void APIOutput::ProfileCreated(Profile profile, QObject*) {
  emit op->toAll(APICommands::CREATE_SETTINGS_PROFILE,
				 Marshalling::toJson(profile));
}

void APIOutput::ProfileCreateError(QString error, QObject* client) {
  Error(APICommands::CREATE_SETTINGS_PROFILE, error, client);
}

void APIOutput::ProfileUpdated(Profile profile, QObject*) {
  emit op->toAll(APICommands::UPDATE_SETTINGS_PROFILE,
				 Marshalling::toJson(profile));
}

void APIOutput::ProfileUpdateError(Profile::ID id, QObject* client) {
  Error(APICommands::UPDATE_SETTINGS_PROFILE, "id=" + id, client);
}

void APIOutput::ProfileDeleted(Profile::ID profile, QObject*) {
  emit op->toAll(APICommands::DELETE_SETTINGS_PROFILE, {{"id", profile}});
}

void APIOutput::ProfileDeleteError(Profile::ID profile, QObject* client) {
  Error(APICommands::DELETE_SETTINGS_PROFILE,
		"Could not delete profile #" + profile, client);
}

void APIOutput::DefaultSettingsProfileSet(Profile::ID profile, QObject*) {
  emit op->toAll(APICommands::SET_DEFAULT_SETTINGS_PROFILE, {{"id", profile}});
}

void APIOutput::DefaultSettingsProfileSetError(QString error, QObject* client) {
  Error(APICommands::SET_DEFAULT_SETTINGS_PROFILE,
		"Could not set profile as default: " + error, client);
}

void APIOutput::SetStartingWell(Job::ID job, Profile::ID plate, int row,
								int col, QObject*) {
  emit op->toAll(
	  APICommands::SET_STARTING_WELL,
	  {{"job_id", job}, {"plate_id", plate}, {"row", row}, {"col", col}});
}

void APIOutput::SetStartingWellError(QString error, QObject* client) {
  Error(APICommands::SET_STARTING_WELL, error, client);
}

void APIOutput::SetColoniesToPick(Job::ID job, QSet<Colony::ID> colonies,
								  QObject* client) {
  // TODO send to all
  QJsonArray cols;

  for (auto it = colonies.begin(); it != colonies.end(); ++it)
	cols << QJsonValue::fromVariant(QVariant::fromValue(*it));  // Qt pls

  emit op->toClient(client, APICommands::SET_COLONIES_TO_PICK,
					{{"job", job}, {"ids", cols}});
}

void APIOutput::SetColoniesToPickError(QString error, QObject* client) {
  Error(APICommands::SET_COLONIES_TO_PICK, error, client);
}

void APIOutput::ColonyDetectionStarted(
	Job::ID, QObject*) { /* TODO inform client, maybe add a loading animtion */
}

void APIOutput::UpdateRequested(QObject* client) {
  emit op->toClient(client, APICommands::UPDATE_SOFTWARE, QJsonObject());
}

void APIOutput::ColonyDetected(Job::ID job, const std::vector<Colony>* colonies,
							   QObject* client) {
  QJsonArray json_coords;

  for (Colony const& colony : *colonies)
	json_coords.push_back(Marshalling::toJson(colony));

  emit op->toClient(client, APICommands::GET_POSITIONS,
					{{"job", job}, {"colonies", json_coords}});
  //	delete colonies;
}

void APIOutput::ColonyDetectionError(QString error, QObject* client) {
  Error(APICommands::GET_POSITIONS, error, client);
}

void APIOutput::VersionSwitched(Version::ID id) {
  emit op->toAll("versionswitched", {{"id", id}});
}

void APIOutput::Error(QString where, QString what, QObject* client) {
  qCritical("Error %s: %s", qPrintable(where), qPrintable(what));
  emit op->toClient(client, "error", {{"where", where}, {"what", what}});
}
}  // namespace c3picko
