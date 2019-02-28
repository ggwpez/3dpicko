#include "include/api_controller.h"
#include "include/algorithm_job.h"
#include "include/algorithm_manager.h"
#include "include/algorithm_result.h"
#include "include/commands/arbitrary_command.h"
#include "include/gcodegenerator.h"
#include "include/octoprint.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QMetaMethod>

using namespace c3picko::pi;
namespace c3picko {
APIController::APIController(AlgorithmManager* detector, Database* db,
                             QObject* parent)
    : QObject(parent),
      detector_(detector),
      db_(db),
      input_(new APIInput(this)),
      output_(new APIOutput(this)) {
  /**
   * The following connects all SIGNALS from APIController (this) prefixed with
   * 'On'
   * to the SLOT with the same name from APIOutput and to defaultSignalHandler.
   * (Pardon my bad style)
   *
   * ex: &ApiController::OnImageListRequested connects to
   *     &APIOutput    ::  ImageListRequested
   */

  QMetaObject const* meta = this->metaObject();
  QMetaMethod default_handler =
      meta->method(meta->indexOfSlot(qPrintable("defaultSignalHandler()")));

  for (int i = meta->methodOffset(); i < meta->methodCount(); ++i) {
    QMetaMethod signal = meta->method(i);
    QString slot_name =
        QMetaObject::normalizedSignature(qPrintable(signal.methodSignature()));

    if (signal.methodType() == QMetaMethod::Signal) {
      if (!slot_name.startsWith("On")) continue;
      slot_name = slot_name.mid(2);
      QObject::connect(this, signal, this, default_handler);

      int slot_index =
          output_->metaObject()->indexOfSlot(qPrintable(slot_name));
      if (slot_index == -1)
        qDebug("Could not find slot APIOutput::%s", qPrintable(slot_name));
      {
        QMetaMethod slot = output_->metaObject()->method(slot_index);

        QObject::connect(this, signal, output_, slot);
        // qDebug("Connecting APIOutput::%s", qPrintable(slot_name));
      }
    }
  }
}

void APIController::defaultSignalHandler() {
  QMetaObject const* meta_sender = QObject::sender()->metaObject();
  QString signal_name = QMetaObject::normalizedSignature(
      meta_sender->method(QObject::senderSignalIndex()).name());

  qDebug("Event %s::%s", qPrintable(meta_sender->className()),
         qPrintable(signal_name));
}

void APIController::DeleteImage(Image::ID id, QObject* client) {
  // Does the image exist?
  if (!db_->images().exists(id)) {
    emit OnImageDeleteError(id, client);
    return;
  }
  // Is the image	used by a job?
  for (auto const& job : db_->jobs()) {
    if (job.imgID() == id) {
      emit OnImageDeleteError(id, client);
      return;
    }
  }

  Image image = db_->images().get(id);
  image.clearCache();

  if (image.deleteFile()) {
    db_->deletedImages().add(id, image);
    db_->images().remove(
        id);  // Carefull here, if we use a reference to image instead, it will
    // go out of scope after deletion
    emit OnImageDeleted(image, client);
  } else {
    emit OnImageDeleteError(image.path(), client);
  }
}

void APIController::DeleteJob(Job::ID id, QObject* client) {
  // Does the job exist?
  if (!db_->jobs().exists(id)) {
    emit OnJobDeleteError(id, client);
  } else {
    Job job = db_->jobs().get(id);
    db_->jobs().remove(id);
    db_->deletedJobs().add(id, job);

    emit OnJobDeleted(job, client);
  }
}

void APIController::UploadImage(Image& image, QObject* client) {
  // Is the user trying to upload an image twice?
  if (db_->images().exists(image.id())) {
    qDebug() << "Ignoring doubled image";
    // TODO inform client?
    emit OnImageCreateError("<cropped>", client);
  } else if (!image.writeToFile()) {
    emit OnImageCreateError(image.id(), client);
  } else {
    db_->images().add(image.id(), image);
    // image.write(response);

    emit OnImageCreated(image, client);
  }
}

void APIController::cropImage(Image::ID id, int x, int y, int w, int h,
                              QObject* client) {
  if (!db_->images().exists(id))
    return emit OnImageCropError(id, client);
  else {
    QString error;
    Image cropped;
    Image& original = db_->images().get(id);

    // Is the cropped image empty?
    if (!original.crop(x, y, w, h, cropped, error)) {
      emit OnImageCropError(id, client);  // TODO inform client
      return;
    }
    if (!cropped.writeToFile())  // save cropped image to the hdd
    {
      emit OnImageCreateError("<cropped>", client);
      return;
    }

    db_->images().add(cropped.id(), cropped);
    // response = {{"id", cropped.id()}}; // TODO
    emit OnImageCropped(cropped, client);
  }
}

void APIController::createSettingsProfile(Profile& profile_wo_id,
                                          QObject* client) {
  Profile::ID id = db_->newProfileId();
  profile_wo_id.setId(id);

  db_->profiles().add(id, profile_wo_id);
  // profile.write(response);
  emit OnProfileCreated(profile_wo_id, client);
}

void APIController::updateSettingsProfile(Profile& profile, QObject* client) {
  if (!db_->profiles().exists(profile.id())) {
    // response = {{"error", "Profile Id unknown: '" +profile.id() +"'"}};
    emit OnProfileUpdateError(profile.id(), client);
  } else {
    db_->profiles().add(profile.id(), profile);
    // response = json_profile;
    emit OnProfileUpdated(profile, client);
  }
}

void APIController::deleteSettingsProfile(Profile::ID id, QObject* client) {
  if (!db_->profiles().exists(id)) {
    emit OnProfileDeleteError(id, client);
  } else {
    // FIXME cant delete profiles used by jobs
    emit OnProfileDeleted(id, client);
    db_->profiles().remove(id);
  }
}

void APIController::setDefaultSettingsProfile(Profile::ID id, QObject* client) {
  if (!db_->profiles().exists(id)) {
    emit OnDefaultSettingsProfileSetError("Profile " + id + " not found",
                                          client);
  } else {
    Profile& profile = db_->profiles().get(id);

    if (profile.type() == "printer-profile")
      db_->setdefaultPrinter(id);
    else if (profile.type() == "socket-profile")
      db_->setDefaultSocket(id);
    else if (profile.type() == "plate-profile")
      db_->setDefaultPlate(id);
    else {
      qCritical() << "Database corrupt or wrong version: Profile" << id
                  << "had unknown type" << profile.type();
      emit OnDefaultSettingsProfileSetError("Database error", client);
      return;
    }

    emit OnDefaultSettingsProfileSet(id, client);
  }
}

void APIController::setStartingWell(Job::ID id, Profile::ID plate_id, int row,
                                    int col, QObject* client) {

  if (!db_->jobs().exists(id)) {
    emit OnSetStartingWellError("Job " + id + " not found", client);
  } else if (!db_->profiles().exists(plate_id)) {
    emit OnSetStartingWellError("Plate " + plate_id + " not found", client);
  } else {
    auto plate = db_->profiles().get(plate_id).plate();

    // Check the validity of row and col
    if (qBound(1, row, plate->numberOfRows()) != row)
      emit OnSetStartingWellError("Invalid starting row " +
                                      QString::number(row) +
                                      " for plate profile: " + plate_id,
                                  client);
    else if (qBound(1, col, plate->numberOfColumns()) != col)
      emit OnSetStartingWellError("Invalid starting col " +
                                      QString::number(row) +
                                      " for plate profile: " + plate_id,
                                  client);
    else {
      Job& job = db_->jobs().get(id);

      job.setStartingRow(row);
      job.setStartingCol(col);
      job.setPlate(plate_id);

      emit OnSetStartingWell(id, plate_id, row, col, client);
    }
  }
}

void APIController::createJob(Job& job, QObject* client) {
  Job::ID id = db_->newJobId();
  job.setCreationDate(QDateTime::currentDateTime());

  //if (!db_->profiles().exists(job.printer()))
    //emit OnJobCreateError("Printer profile " + id + " unknown", client);
  //else if (!db_->profiles().exists(job.socket()))
    //emit OnJobCreateError("Socket profile " + id + " unknown", client);
  //else {
    job.setId(id);
    db_->jobs().add(id, job);
    emit OnJobCreated(job, client);
  //}
}

void APIController::getPositions(Image::ID id, QObject* client) {
  // AlgorithmJob* job = detectColonies(job_id, defaultAlgo(), {}, client);

  // if (job)
  // job->start(true, true);
}

AlgorithmJob* APIController::detectColonies(Job::ID job_id, QString algo_id,
                                            QJsonObject settings,
                                            QObject* client) {
  if (!db_->jobs().exists(job_id)) {
    qWarning() << "Client requested colony detection for unknown job, id="
               << job_id;
    emit OnColonyDetectionError("Job not found", client);
  } else {
    Job& job = db_->jobs().get(job_id);
    Image::ID img_id = job.imgID();

    if (!db_->images().exists(img_id)) {
      qWarning() << "Colony detection error: image not found (#" << img_id
                 << ")";
      emit OnColonyDetectionError("Image not found", client);
    } else {
      Image& img = db_->images().get(
          img_id);  // Non const& bc readCvMat() can set the cache
      cv::Mat image;

      if (!img.readCvMat(image)) {
        qCritical() << "CV could not read image" << img.path();
        emit OnColonyDetectionError("Image not readable or empty", client);
      } else {
        AlgorithmJob::ID result_job_id = db_->newResultJobId();
        AlgorithmResult::ID result_id = db_->newResultId();
        job.setResultID(result_id);
        AlgorithmJob* algo_job = detector_->createJob(
            image, algo_id, result_job_id, result_id, settings);

        if (!algo_job) {
          emit OnColonyDetectionError("Algorithm not found", client);
        } else {
          connect(algo_job, &AlgorithmJob::OnFinished, client,
                  [this, client, algo_job, result_id]() {
                    db_->detectionResults().add(
                        result_id, *algo_job->result());  // TODO not too cool
                    emit this->OnColonyDetected(&algo_job->result()->colonies_,
                                                client);
                  });

          return algo_job;
        }
      }
    }
  }
  return nullptr;
}

void APIController::updateDetectionSettings(Job::ID job_id, QString algo_id,
                                            QJsonObject settings,
                                            QObject* client) {
  AlgorithmJob* job = detectColonies(job_id, algo_id, settings, client);

  if (job) job->start(true, true);
}

void APIController::startJob(Job::ID id, QObject* client) {
  static OctoConfig config("192.168.2.2", "F866D6261972458CACAE9CB56E484758");
  static OctoPrint* printer = new OctoPrint(config, this);

  if (!db_->jobs().exists(id)) {
    emit OnColonyDetectionError("Job '" + id + "' job found", client);
    return;
  }
  Job& job = db_->jobs().get(id);

  if (!db_->detectionResults().exists(job.resultID())) {
    emit OnColonyDetectionError(
        "Job " + id +
            " could not find its detection results: " + job.resultID(),
        client);
    return;
  }
  AlgorithmResult& detection = db_->detectionResults().get(job.resultID());

  // FIXME check
  PrinterProfile* printerp =
      (PrinterProfile*)(db_->profiles().get(job.printer()));
  PlateSocketProfile* socket =
      (PlateSocketProfile*)(db_->profiles().get(job.socket()));
  PlateProfile* plate = (PlateProfile*)(db_->profiles().get(job.plate()));

  GcodeGenerator gen(*socket, *printerp, *plate);

  std::vector<Colony> colonies =
      detection.colonies();  // FIXME convert to local coordinates
  std::vector<LocalColonyCoordinates> coords;
  for (int i = 0; i < colonies.size(); ++i)
    coords.push_back(Point(colonies[i].x() * 127, (1.0d -colonies[i].y()) * 85));

  auto code = gen.CreateGcodeForTheEntirePickingProcess(
      job.startingRow(), job.startingCol(), coords);
  std::ostringstream s;

  QFile file("out.gcode");
  if (!file.open(QIODevice::WriteOnly))
    throw std::runtime_error("Could not save gcode");
  QTextStream ts(&file);

  QStringList gcode_list;
  for (auto c : code) {
    ts << QString::fromStdString(c.ToString()) << endl;
    gcode_list << QString::fromStdString(c.ToString());
  }

  Command* cmd = commands::ArbitraryCommand::MultiCommand(gcode_list);

  printer->SendCommand(cmd);
}

void APIController::shutdown(QObject*) {
  qDebug() << "Shutdown";
  qApp->exit(0);
}

void APIController::restart(QObject*) {
  qDebug() << "Restart";
  qApp->exit(123);
}

QJsonObject APIController::createImageList() const {
  QJsonArray json_images;

  // Get all image ids
  auto ids = db_->images().entries().keys();

  // Order ids by their matching images upload date
  std::sort(ids.begin(), ids.end(), [this](Image::ID a, Image::ID b) {
    return this->db_->images().entries()[a].uploaded() <
           this->db_->images().entries()[b].uploaded();
  });

  for (auto id : ids)
    json_images.push_back(Marshalling::toJson(db_->images().get(id)));

  return {{"images", json_images}};
}

QJsonObject APIController::createImageList(Image img) {
  QJsonArray json_images;

  json_images.append(Marshalling::toJson(img));

  return {{"images", json_images}};
}

QJsonObject APIController::createJobList() {
  QJsonArray json_jobs;

  for (auto const& job : db_->jobs())
    json_jobs.append(Marshalling::toJson(job));

  return {{"jobs", json_jobs}};
}

QJsonObject APIController::createJobList(Job job) {
  QJsonArray json_jobs;

  json_jobs.append(Marshalling::toJson(job));

  return {{"jobs", json_jobs}};
}

QJsonObject APIController::createProfileList() {
  QJsonObject json;
  QJsonArray json_profiles;

  for (auto const& profile : db_->profiles())
    json_profiles.append(Marshalling::toJson(profile));

  json["defaultPrinter"] = db_->defaultPrinter();
  json["defaultSocket"] = db_->defaultSocket();
  json["defaultPlate"] = db_->defaultPlate();

  json["profiles"] = json_profiles;

  json["printerTemplate"] = Profile::printerTemplate();
  json["socketTemplate"] = Profile::socketTemplate();
  json["plateTemplate"] = Profile::plateTemplate();
  // json["octoprintTemplate"] = ;

  return json;
}

QJsonObject APIController::CreateAlgorithmList() {
  QJsonObject json;

  for (Algorithm* algo : detector_->algos())
    json[algo->id()] = Marshalling::toJson(*algo);

  return json;
}

QJsonObject APIController::createDeleteImage(Image img) {
  return {{"id", img.id()}};
}

QJsonObject APIController::createDeleteJob(Job job) {
  return {{"id", job.id()}};
}

void APIController::request(QJsonObject request, QString raw_request,
                            QObject* client) {
  try {
    input_->serviceRequest(request, raw_request, client);
  } catch (std::exception const& e) {
    output_->Error("Exception", e.what(), client);
    qWarning("std::exception %s", e.what());
  } catch (...) {
    output_->Error("Exception", "unknown", client);
    qWarning("unknown exception");
  }
}

Database& APIController::db() const { return *db_; }
}  // namespace c3picko
