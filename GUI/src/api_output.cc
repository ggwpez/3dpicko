#include "include/api_output.h"
#include "include/api_controller.h"
#include "include/colonydetector.h"
#include <QJsonArray>
#include <QThread>

namespace c3picko {

APIOutput::APIOutput(APIController *parent)
    : QObject((QObject *)parent), op(parent) {}

void APIOutput::UnknownRequest(QString request, QObject *client) {}

void APIOutput::ImageListRequested(QObject *client) {
  emit op->toClient(client, "getimagelist", op->createImageList());
}

void APIOutput::JobListRequested(QObject *client) {
  emit op->toClient(client, "getjoblist", op->createJobList());
}

void APIOutput::ProfileListRequested(QObject *client) {
  emit op->toClient(client, "getprofilelist", op->createProfileList());
}

void APIOutput::AlgorithmListRequested(QObject *client) {
  emit op->toClient(client, "getdetectionalgorithms",
                    op->CreateAlgorithmList());
}

void APIOutput::JobCreated(Job job, QObject *client) {
  QJsonObject json(job);

  emit op->toClient(client, "createjob", json);
  emit op->toAllExClient(client, "getjoblist", json); // TODO lazy
}

void APIOutput::JobCreateError(QString id, QObject *client) {
  Error("jobcreate", "", client);
}

void APIOutput::JobDeleted(Job job, QObject *client) {
  emit op->toAll("deletejob", {{"id", job.id()}});
}

void APIOutput::JobDeleteError(QString path, QObject *client) {
  Error("jobdelete", "", client);
}

void APIOutput::ImageCreated(Image image, QObject *client) {
  emit op->toAll("uploadimage", (QJsonObject)image);
}

void APIOutput::ImageCreateError(QString path, QObject *client) {
  Error("uploadimage", "", client);
}

void APIOutput::ImageDeleted(Image image, QObject *client) {
  emit op->toAll("deleteimage", (QJsonObject)image);
}

void APIOutput::ImageDeleteError(QString path, QObject *client) {
  Error("deleteimage", "", client);
}

void APIOutput::ImageCropped(Image image, QObject *client) {
  QJsonObject json(image);

  emit op->toClient(client, "crop-image", json);
  emit op->toAllExClient(client, "getimagelist", json);
}

void APIOutput::ImageCropError(QString id, QObject *client) {
  Error("crop-image", "", client);
}

void APIOutput::ProfileCreated(Profile profile, QObject *client) {
  emit op->toAll("createsettingsprofile", (QJsonObject)profile);
}

void APIOutput::ProfileUpdated(Profile profile, QObject *client) {
  emit op->toAll("updatesettingsprofile", (QJsonObject)profile);
}

void APIOutput::ProfileUpdateError(Profile::ID profile, QObject *client) {
  Error("updatesettingsprofile", "", client);
}

void APIOutput::ProfileDeleted(Profile::ID profile, QObject *client) {
  emit op->toAll("deletesettingsprofile", {{"id", profile}});
}

void APIOutput::ProfileDeleteError(Profile::ID profile, QObject *client) {
  Error("deletesettingsprofile", "Could not delete profile #" + profile,
        client);
}

void APIOutput::ColonyDetected(ColonyDetector *detector, QObject *client) {
  std::vector<cv::Vec3f> const &coords = detector->pos();
  QJsonArray json_coords;

  for (int i = 0; i < coords.size(); ++i) {
    json_coords.push_back(
        QJsonArray({coords[i][0], coords[i][1], coords[i][2]}));
  }

  emit op->toClient(client, "getpositions", {{"coords", json_coords}});
}

void APIOutput::ColonyDetectionError(QString error, QObject *client) {
  Error("getpositions", "Could not detect colonies", client);
}

void APIOutput::Error(QString where, QString what, QObject *client) {
  emit op->toClient(client, "error", {{where, what}});
}
} // namespace c3picko
