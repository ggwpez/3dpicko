#include "include/api_input.h"
#include "include/api_controller.h"

namespace c3picko {

APIInput::APIInput(APIController* parent) : QObject(parent), api(parent) {}

void APIInput::serviceRequest(QJsonObject& request, QString const& raw_request,
                              QObject* client) {
  QString path = Marshalling::fromJson<QString>(request["request"]).toLower();
  QJsonObject req_data = request["data"].toObject();

  // All no-brainer requests are directly emittet as signals, indicating
  // success.
  // Complex request are parsed and forwarded to the api_controller.
  if (path == "getimagelist") {
    emit api->OnImageListRequested(client);
  } else if (path == "getprofilelist") {
    emit api->OnProfileListRequested(client);
  } else if (path == "getjoblist") {
    emit api->OnJobListRequested(client);
  } else if (path == "getdetectionalgorithms") {
    emit api->OnAlgorithmListRequested(client);
  } else if (path == "setdefaultsettingsprofile") {
    Profile::ID id = Marshalling::fromJson<QString>(req_data["id"]);
    api->setDefaultSettingsProfile(id, client);
  } else if (path == "setstartingwell") {
    Job::ID job_id = Marshalling::fromJson<QString>(req_data["job_id"]);
    Profile::ID plate_id = Marshalling::fromJson<QString>(req_data["plate_id"]);
    int row = req_data["row"].toInt();
    int col = req_data["column"].toInt();

    api->setStartingWell(job_id, plate_id, row, col, client);
  } else if (path == "deleteimage") {
    Image::ID id = Marshalling::fromJson<QString>(req_data["id"]);

    api->DeleteImage(id, client);
  } else if (path == "deletejob") {
    QString id = Marshalling::fromJson<QString>(req_data["id"]);
    api->DeleteJob(id, client);
  } else if (path == "startjob") {
    Job::ID job = Marshalling::fromJson<QString>(req_data["id"]);

    api->startJob(job, client);
  } else if (path == "uploadimage") {
    // Get image data
    QByteArray img_data(
        QByteArray::fromBase64(Marshalling::fromJson<QString>(req_data["file"])
                                   .toUtf8()));  // TODO ugly code
    QString img_name =
        Marshalling::fromJson<QString>(req_data["original_filename"]);
    Image image =
        Image(img_data, img_name, "descrtiption", QDateTime::currentDateTime());
    qDebug() << "Hash" << image.id();

    api->UploadImage(image, client);
  } else if (path == "crop-image") {
    QString img_id = Marshalling::fromJson<QString>(req_data["id"]);
    int x = req_data["x"].toDouble(), y = req_data["y"].toDouble(),
        w = req_data["width"].toDouble(), h = req_data["height"].toDouble();

    api->cropImage(img_id, x, y, w, h, client);
  } else if (path == "createsettingsprofile") {
    // Profile::ID newId		 = api->db().newProfileId();

    // json_profile["id"] = newId; // TODO  hack
    Profile profile_wo_id(Marshalling::fromJson<Profile>(req_data));

    api->createSettingsProfile(profile_wo_id, client);
  } else if (path == "updatesettingsprofile") {
    Profile profile(Marshalling::fromJson<Profile>(req_data));

    api->updateSettingsProfile(profile, client);
  } else if (path == "deletesettingsprofile") {
    Profile::ID id = Marshalling::fromJson<QString>(req_data["id"]);

    api->deleteSettingsProfile(id, client);
  } else if (path == "createjob") {
    Job job_wo_id(Marshalling::fromJson<Job>(req_data));

    api->createJob(job_wo_id, client);
  } else if (path == "getpositions") {
    QString img_id = Marshalling::fromJson<QString>(req_data["id"]);
    api->getPositions(img_id, client);
  } else if (path == "updatedetectionsettings") {
    Job::ID job_id = Marshalling::fromJson<QString>(req_data["job_id"]);
    QString algo_id = Marshalling::fromJson<QString>(req_data["algorithm"]);
    QJsonObject settings = req_data["settings"].toObject();

    emit api->OnColonyDetectionStarted(job_id, client);
    api->updateDetectionSettings(job_id, algo_id, settings, client);
  } else if (path == "shutdown") {
    api->shutdown(client);
  } else if (path == "restart") {
    api->restart(client);
  } else if (path == "backup") {
    api->db().saveToFile();
  } else {
    qWarning() << "Unknown request" << raw_request;
    emit api->OnUnknownRequest("", client);
    // response = {{"error", "unknown request"}};
  }
}
}  // namespace c3picko
