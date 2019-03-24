#include "include/api_output.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QVariant>
#include "include/api_controller.h"
#include "include/colony.hpp"

namespace c3picko {
APIOutput::APIOutput(APIController* parent)
    : QObject((QObject*)parent), op(parent) {}

void APIOutput::UnknownRequest(QString request, QObject* client) {
  Error("Unknown request", request, client);
}

void APIOutput::ImageListRequested(QObject* client) {
  emit op->toClient(client, "getimagelist", op->createImageList());
}

void APIOutput::JobListRequested(QObject* client) {
  emit op->toClient(client, "getjoblist", op->createJobList());
}

void APIOutput::ProfileListRequested(QObject* client) {
  emit op->toClient(client, "getprofilelist", op->createProfileList());
}

void APIOutput::AlgorithmListRequested(QObject* client) {
  emit op->toClient(client, "getdetectionalgorithms",
                    op->CreateAlgorithmList());
}

void APIOutput::JobCreated(Job job, QObject* client) {
  QJsonObject json(Marshalling::toJson(job));

  emit op->toClient(client, "createjob", json);
  emit op->toAllExClient(client, "getjoblist", json);  // TODO lazy
}

void APIOutput::JobCreateError(QString id, QObject* client) {
  Error("jobcreate", id, client);
}

void APIOutput::JobDeleted(Job job, QObject*) {
  emit op->toAll("deletejob", {{"id", job.id()}});
}

void APIOutput::JobDeleteError(Job::ID id, QObject* client) {
  Error("jobdelete", "id=" + id, client);
}

void APIOutput::JobStarted(Job::ID id, QObject*) {
  emit op->toAll("startjob", {{"id", id}});
}

void APIOutput::JobStartError(QString error, QObject* client) {
  Error("startjob", error, client);
}

void APIOutput::ImageCreated(Image image, QObject*) {
  emit op->toAll("uploadimage", Marshalling::toJson(image));
}

void APIOutput::ImageCreateError(QString path, QObject* client) {
  Error("uploadimage", "path=" + path, client);
}

void APIOutput::ImageDeleted(Image image, QObject*) {
  emit op->toAll("deleteimage", {{"id", image.id()}});
}

void APIOutput::ImageDeleteError(QString path, QObject* client) {
  Error("deleteimage", "path=" + path, client);
}

void APIOutput::ImageCropped(Image image, QObject* client) {
  QJsonObject json(Marshalling::toJson(image));

  emit op->toClient(client, "crop-image", json);
  emit op->toAllExClient(client, "getimagelist", json);
}

void APIOutput::ImageCropError(QString id, QObject* client) {
  Error("crop-image", "id=" + id, client);
}

void APIOutput::ProfileCreated(Profile profile, QObject*) {
  emit op->toAll("createsettingsprofile", Marshalling::toJson(profile));
}

void APIOutput::ProfileUpdated(Profile profile, QObject*) {
  emit op->toAll("updatesettingsprofile", Marshalling::toJson(profile));
}

void APIOutput::ProfileUpdateError(Profile::ID id, QObject* client) {
  Error("updatesettingsprofile", "id=" + id, client);
}

void APIOutput::ProfileDeleted(Profile::ID profile, QObject*) {
  emit op->toAll("deletesettingsprofile", {{"id", profile}});
}

void APIOutput::ProfileDeleteError(Profile::ID profile, QObject* client) {
  Error("deletesettingsprofile", "Could not delete profile #" + profile,
        client);
}

void APIOutput::DefaultSettingsProfileSet(Profile::ID profile, QObject*) {
  emit op->toAll("setdefaultsettingsprofile", {{"id", profile}});
}

void APIOutput::DefaultSettingsProfileSetError(QString error, QObject* client) {
  Error("setdefaultsettingsprofile",
        "Could not set profile as default: " + error, client);
}

void APIOutput::SetStartingWell(Job::ID job, Profile::ID plate, int row,
                                int col, QObject*) {
  emit op->toAll(
      "setstartingwell",
      {{"job_id", job}, {"plate_id", plate}, {"row", row}, {"col", col}});
}

void APIOutput::SetStartingWellError(QString error, QObject* client) {
  Error("setstartingwell", error, client);
}

void APIOutput::SetColoniesToPick(Job::ID job, QSet<Colony::ID> colonies,
                                  QObject* client) {
  // TODO send to all
  QJsonArray cols;

  for (auto it = colonies.begin(); it != colonies.end(); ++it)
    cols << QJsonValue::fromVariant(QVariant::fromValue(*it));  // Qt pls

  emit op->toClient(client, "setcoloniestopick", {{"job", job}, {"ids", cols}});
}

void APIOutput::SetColoniesToPickError(QString error, QObject* client) {
  Error("setColoniesToPick", error, client);
}

void APIOutput::ColonyDetectionStarted(
    Job::ID, QObject*) { /* TODO inform client, maybe add a loading animtion */
}

void APIOutput::ColonyDetected(const std::vector<Colony>* colonies,
                               QObject* client) {
  QJsonArray json_coords;

  for (Colony const& colony : *colonies)
    json_coords.push_back(Marshalling::toJson(colony));

  emit op->toClient(client, "getpositions", {{"colonies", json_coords}});
  //	delete colonies;
}

void APIOutput::ColonyDetectionError(QString error, QObject* client) {
  Error("getpositions", error, client);
}

void APIOutput::Error(QString where, QString what, QObject* client) {
  qCritical("Error %s: %s", qPrintable(where), qPrintable(what));
  emit op->toClient(client, "error", {{"where", where}, {"what", what}});
}
}  // namespace c3picko
