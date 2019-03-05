#include "include/api_output.h"
#include <QJsonArray>
#include "include/api_controller.h"
#include "include/colony.hpp"

#include <QJsonDocument>

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
  qDebug("JobCreateError %s", qPrintable(id));
}

void APIOutput::JobDeleted(Job job, QObject* client) {
  emit op->toAll("deletejob", {{"id", job.id()}});
}

void APIOutput::JobDeleteError(QString path, QObject* client) {
  Error("jobdelete", "", client);
}

void APIOutput::ImageCreated(Image image, QObject* client) {
  emit op->toAll("uploadimage", Marshalling::toJson(image));
}

void APIOutput::ImageCreateError(QString path, QObject* client) {
  Error("uploadimage", "", client);
}

void APIOutput::ImageDeleted(Image image, QObject* client) {
  emit op->toAll("deleteimage", Marshalling::toJson(image));
}

void APIOutput::ImageDeleteError(QString path, QObject* client) {
  Error("deleteimage", "", client);
}

void APIOutput::ImageCropped(Image image, QObject* client) {
  QJsonObject json(Marshalling::toJson(image));

  emit op->toClient(client, "crop-image", json);
  emit op->toAllExClient(client, "getimagelist", json);
}

void APIOutput::ImageCropError(QString id, QObject* client) {
  Error("crop-image", "", client);
}

void APIOutput::ProfileCreated(Profile profile, QObject* client) {
  emit op->toAll("createsettingsprofile", Marshalling::toJson(profile));
}

void APIOutput::ProfileUpdated(Profile profile, QObject* client) {
  emit op->toAll("updatesettingsprofile", Marshalling::toJson(profile));
}

void APIOutput::ProfileUpdateError(Profile::ID profile, QObject* client) {
  Error("updatesettingsprofile", "", client);
}

void APIOutput::ProfileDeleted(Profile::ID profile, QObject* client) {
  emit op->toAll("deletesettingsprofile", {{"id", profile}});
}

void APIOutput::ProfileDeleteError(Profile::ID profile, QObject* client) {
  Error("deletesettingsprofile", "Could not delete profile #" + profile,
        client);
}

void APIOutput::DefaultSettingsProfileSet(Profile::ID profile,
                                          QObject* client) {
  emit op->toAll("setdefaultsettingsprofile", {{"id", profile}});
}

void APIOutput::DefaultSettingsProfileSetError(QString error, QObject* client) {
  Error("setdefaultsettingsprofile",
        "Could not set profile as default: " + error, client);
}

void APIOutput::SetStartingWell(Job::ID job, Profile::ID plate, int row,
                                int col, QObject* client) {
  emit op->toAll(
      "setstartingwell",
      {{"job_id", job}, {"plate_id", plate}, {"row", row}, {"col", col}});
}

void APIOutput::SetStartingWellError(QString error, QObject* client) {
  Error("setstartingwell", error, client);
}

void APIOutput::ColonyDetectionStarted(
    Job::ID,
    QObject* client) { /* TODO inform client, maybe add a loading animtion */
}

void APIOutput::ColonyDetected(std::vector<Colony>* colonies, QObject* client) {
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
  emit op->toClient(client, "error", {{where, what}});
}
}  // namespace c3picko
