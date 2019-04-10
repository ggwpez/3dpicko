#include "include/api_controller.h"
#include "include/algorithm_job.h"
#include "include/algorithm_manager.h"
#include "include/algorithm_result.h"
#include "include/commands/arbitrary_command.h"
#include "include/detection_result.h"
#include "include/exception.h"
#include "include/gcodegenerator.h"
#include "include/octoprint.h"
#include "include/plate_result.h"
#include "include/reporter.h"
#include "include/types/well.h"
#include "include/version_manager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QMetaMethod>
#include <QPdfWriter>
#include <memory>
#include <random>

using namespace c3picko::pi;
namespace c3picko {
APIController::APIController(AlgorithmManager* colony_detector,
                             AlgorithmManager* plate_detector,
                             VersionManager* version_manager, Database* db,
                             QObject* parent)
    : QObject(parent),
      colony_detector_(colony_detector),
      plate_detector_(plate_detector),
      version_manager_(version_manager),
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
        throw Exception("No such slot APIOutput::" + slot_name);
      else {
        QMetaMethod slot = output_->metaObject()->method(slot_index);
        QObject::connect(this, signal, output_, slot);
      }
    }
  }

  // Connect the signals from version_manager_ to our slots, in order to be able
  // to inform the client, when new updates are ready
  if (version_manager) {
    /*connect(version_manager, &VersionManager::OnInstallBegin, this,
    &APIController::versionInstallBegin); connect(version_manager,
    &VersionManager::OnInstalled, this, &APIController::versionInstalled);
    connect(version_manager, &VersionManager::OnUnInstalled, this,
    &APIController::versionUnInstalled); connect(version_manager,
    &VersionManager::OnInstallError, this,
    &APIController::versionInstallError);*/
    connect(version_manager, &VersionManager::OnSwitched, this,
            &APIController::OnVersionSwitched);
  }
}

void APIController::defaultSignalHandler() {
  QMetaObject const* meta_sender = QObject::sender()->metaObject();
  QString signal_name = QMetaObject::normalizedSignature(
      meta_sender->method(QObject::senderSignalIndex()).name());
  qDebug("Event %s::%s", qPrintable(meta_sender->className()),
         qPrintable(signal_name));
}

QJsonObject APIController::createVersionList() const {
  QJsonObject ret;

  QJsonArray all;
  for (Version const& version : db_->versions())
    all.push_back(Marshalling::toJson(version));

  QJsonArray installed;
  for (Version::ID id : db_->installedVersions()) installed.push_back(id);

  ret["all"] = all;
  ret["installed"] = installed;
  ret["current"] = currentVersion().id();
  if (version_manager_)
    ret["selected"] = version_manager_->selected();
  else
    ret["selected"] = currentVersion().id();

  return ret;
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
    emit OnImageDeleteError(image.path().toSystemAbsolute(), client);
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

void APIController::UploadImage(Image image, QObject* client) {
  cv::Mat raw;
  if (!image.readCvMat(raw)) {
    emit OnImageCreateError("Could not read/find image", client);
    return;
  }

  Algorithm::ID aid("1");
  AlgorithmJob::ID jid = db_->newResultJobId();
  std::shared_ptr<PlateResult> result(
      std::make_shared<PlateResult>(db_->newResultId()));

  AlgorithmJob* ajob =
      plate_detector_->createJob(raw, aid, jid, result, QJsonObject());

  if (!ajob)
    emit OnImageCreateError("Internal Error: Algorithm not found (" + aid + ")",
                            client);
  else {
    // Copy the result shared_ptr, such that it lives until we are done with
    // the callback.
    // Here we crop the image and insert it into the database, it not yet
    // existing
    connect(ajob, &AlgorithmJob::OnAlgoSucceeded, ajob,
            [this, client, image, ajob, result] {
              if (!result->rotatedImage())
                return emit OnImageCreateError(
                    "Assertion failure: Result from PlateDetection was null",
                    client);

              Image cropped(*result->rotatedImage(), image.originalName(),
                            image.description(), image.uploaded());

              if (!cropped.writeToFile())
                return emit OnImageCreateError(cropped.id(), client);

              // Is the user trying to upload an image twice?
              if (db_->images().exists(cropped.id()))
                qDebug() << "Ignoring doubled image";
              else
                db_->images().add(cropped.id(), cropped);

              emit this->OnImageCreated(cropped, client);
              qDebug() << "Detected Plate in" << ajob->tookMs() << "ms";
            });
    connect(ajob, &AlgorithmJob::OnAlgoFailed, ajob, [this, ajob, client] {
      emit OnImageCreateError(ajob->result()->stageError(), client);
    });

    ajob->start(true, true);
  }
}

void APIController::createSettingsProfile(Profile& profile_wo_id,
                                          QObject* client) {
  Profile::ID id = db_->newProfileId();
  profile_wo_id.setId(id);

  // check that there is not already an octoprint profile
  if (profile_wo_id.type() == ProfileType::OCTOPRINT) {
    for (auto it = db_->profiles().begin(); it != db_->profiles().end(); ++it)
      if (it->type() == ProfileType::OCTOPRINT)
        return emit OnProfileCreateError(
            "Cant create more than one octoprint profile", client);
  }

  db_->profiles().add(id, profile_wo_id);
  emit OnProfileCreated(profile_wo_id, client);
}

void APIController::updateSettingsProfile(Profile& profile, QObject* client) {
  if (!db_->profiles().exists(profile.id())) {
    emit OnProfileUpdateError(profile.id(), client);
  } else {
    db_->profiles().add(profile.id(), profile);
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
    Profile const& profile = db_->profiles().get(id);

    if (profile.type() == ProfileType::PRINTER)
      db_->setdefaultPrinter(id);
    else if (profile.type() == ProfileType::SOCKET)
      db_->setDefaultSocket(id);
    else if (profile.type() == ProfileType::PLATE)
      db_->setDefaultPlate(id);
    else if (profile.type() == ProfileType::OCTOPRINT)
      db_->setDefaultOctoprint(id);
    else {
      qCritical() << "Database corrupt or wrong version: Profile" << id
                  << "had unknown type" << (int)profile.type();
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

void APIController::setColoniesToPick(Job::ID id, QSet<Colony::ID> ex_user,
                                      QSet<Colony::ID> in_user, quint32 number,
                                      QObject* client) {
  if (!db_->jobs().exists(id))
    return emit OnSetColoniesToPickError("Job " + id + " not found", client);

  Job& job = db_->jobs().get(id);
  if (!job.resultJob() || !job.resultJob()->result())
    return emit OnSetColoniesToPickError(
        "Job " + id + " has no detected colonies attatched to it", client);

  DetectionResult* result =
      static_cast<DetectionResult*>(job.resultJob()->result().get());
  QSet<Colony::ID> suitable;
  QSet<Colony::ID> in_algo, ex_algo;
  {
    for (auto it = result->includedBegin(); it != result->includedEnd(); ++it)
      in_algo.insert(it->id());
    for (auto it = result->excludedBegin(); it != result->excludedEnd(); ++it)
      ex_algo.insert(it->id());
  }

  // Input validation
  {
    if (!(ex_user & ex_algo).empty())
      return emit OnSetColoniesToPickError(
          "Cant exclude colonies that are already excluded by the "
          "algorithm",
          client);
    if (!(ex_user - in_algo).empty())
      return emit OnSetColoniesToPickError(
          "Cant exclude colonies that are not included by the algorithm",
          client);
    if (!(in_user & in_algo).empty())
      return emit OnSetColoniesToPickError(
          "Cant include colonies that are already included by the "
          "algorithm",
          client);
    if (!(in_user - ex_algo).empty())
      return emit OnSetColoniesToPickError(
          "Cant include colonies that are not excluded by the algorithm",
          client);
    if (!(in_user & ex_user).empty())
      return emit OnSetColoniesToPickError(
          "Cant include and exclude a colony at the same time", client);
  }

  // Calculte all suitable colonies and remove all-'number' many of them
  // randomly
  suitable = (in_algo - ex_user) - (ex_algo - in_user);
  quint32 all = suitable.size();

  if (number > all)
    return emit OnSetColoniesToPickError(
        "Cant pick more colonies than available", client);

  std::mt19937_64 gen(1234);  // same seed for determinism
  for (quint32 i = 0; i < all - number; ++i) {
    std::uniform_int_distribution<> dis(0, suitable.size() - 1);
    suitable.erase(suitable.begin() + dis(gen));
  }

  job.setcoloniesToPick(suitable);
  emit OnSetColoniesToPick(id, suitable, client);
}

void APIController::createJob(Job& job, QObject* client) {
  Job::ID id = db_->newJobId();
  job.setCreationDate(QDateTime::currentDateTime());

  // if (!db_->profiles().exists(job.printer()))
  // emit OnJobCreateError("Printer profile " + id + " unknown", client);
  // else if (!db_->profiles().exists(job.socket()))
  // emit OnJobCreateError("Socket profile " + id + " unknown", client);
  // else {
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

std::shared_ptr<AlgorithmJob> APIController::detectColonies(
    Job::ID job_id, QString algo_id, QJsonObject settings, QObject* client) {
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
        qCritical() << "CV could not read image"
                    << img.path().toSystemAbsolute();
        emit OnColonyDetectionError("Image not readable or empty", client);
      } else {
        AlgorithmResult::ID result_id = db_->newResultId();
        AlgorithmJob::ID algo_job_id = db_->newResultJobId();
        std::shared_ptr<DetectionResult> result(
            std::make_shared<DetectionResult>(result_id));
        std::shared_ptr<AlgorithmJob> algo_job(colony_detector_->createJob(
            image, algo_id, algo_job_id, result, settings));
        AlgorithmJob* raw = algo_job.get();

        if (!algo_job) {
          emit OnColonyDetectionError("Algorithm not found", client);
        } else {
          job.setResultAlgorithmJob(algo_job);
          connect(raw, &AlgorithmJob::OnAlgoSucceeded, raw,
                  [this, client, job_id, raw, result] {
                    emit this->OnColonyDetected(job_id, &result->colonies(),
                                                client);
                    qDebug() << "Detected" << result->colonies().size() << "in"
                             << raw->tookMs() << "ms";
                  });
          connect(algo_job.get(), &AlgorithmJob::OnAlgoFailed, raw,
                  [this, raw, client] {
                    emit OnColonyDetectionError(raw->result()->stageError(),
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
  std::shared_ptr<AlgorithmJob> job =
      detectColonies(job_id, algo_id, settings, client);

  if (job) job->start(true, false);
}

void APIController::startJob(Job::ID id, Profile::ID octoprint_id,
                             QObject* client) {
  if (!db_->jobs().exists(id))
    return emit OnJobStartError("Job '" + id + "' job found", client);
  Job& job = db_->jobs().get(id);

  if (!job.resultJob() || !job.resultJob()->result())
    return emit OnJobStartError(
        "Job " + id +
            " could not find its detection results: " + job.resultID(),
        client);
  DetectionResult* result =
      static_cast<DetectionResult*>(job.resultJob()->result().get());

  if (!db_->profiles().exists(job.printer()) ||
      !db_->profiles().exists(job.socket()) ||
      !db_->profiles().exists(job.plate()) ||
      !db_->profiles().exists(octoprint_id))
    return emit OnJobStartError(
        "Internal error: Cant find printer, socket, plate or octoprint profile",
        client);
  job.setOctoprint(octoprint_id);

  PrinterProfile* printerp =
      db_->profiles().get(job.printer()).operator c3picko::PrinterProfile*();
  PlateSocketProfile* socket =
      db_->profiles().get(job.socket()).operator c3picko::PlateSocketProfile*();
  PlateProfile* plate =
      db_->profiles().get(job.plate()).operator c3picko::PlateProfile*();
  OctoConfig* octoprint =
      db_->profiles().get(job.octoprint()).operator c3picko::pi::OctoConfig*();

  OctoPrint* printer = new OctoPrint(*octoprint, this);
  GcodeGenerator gen(*socket, *printerp, *plate);

  // Order the colonies by their top left position
  QSet<Colony::ID> selected = job.coloniesToPick();

  std::vector<LocalColonyCoordinates> coords;
  std::map<Well, Colony::ID> pick_positions;

  Well well(job.startingRow(), job.startingCol(), plate);
  // Convert the colony coordinates to real world coordinates
  for (auto it = selected.begin(); it != selected.end(); ++it) {
    auto f = std::find_if(result->includedBegin(), result->includedEnd(),
                          [&it](Colony const& c) { return c.id() == *it; });

    if (f == result->includedEnd())
      return emit OnJobStartError("Internal error: Cant find selected colonie",
                                  client);

    // Invert the y-axis. FIXME get the frame size from the plate profile
    coords.push_back(Point(f->x() * 128, (1.0 - f->y()) * 85.9));
    pick_positions.emplace(well, *it);
    if (it + 1 != selected.end()) ++well;
  }

  std::vector<GcodeInstruction> code =
      gen.CreateGcodeForTheEntirePickingProcess(job.startingRow(),
                                                job.startingCol(), coords);

  Reporter reporter(Reporter::fromDatabase(*db_, db_->newReportId(), job.id(),
                                           pick_positions));
  Report report = reporter.createReport();
  job.setReportPath(report.data());

  QFile file("out.gcode");
  if (!file.open(QIODevice::WriteOnly)) qWarning() << "Could not save gcode";
  QTextStream ts(&file);

  QStringList gcode_list;
  for (auto const& c : code) {
    ts << QString::fromStdString(c.ToString()) << endl;
    gcode_list << QString::fromStdString(c.ToString());
  }
  file.flush();
  file.close();

  emit OnJobStarted(report, client);
  Command* cmd = commands::ArbitraryCommand::MultiCommand(gcode_list);
  printer->SendCommand(cmd);  // TODO inform client
  connect(cmd, &Command::OnFinished, printer, &OctoPrint::deleteLater);
}

void APIController::shutdown(QObject*) {
  qDebug() << "Shutdown";
  qApp->exit(EXIT_SUCCESS);
}

void APIController::restart(QObject*) {
  qDebug() << "Restart";
  qApp->exit(exitCodeHardRestart());
}

QJsonObject APIController::createImageList() const {
  QJsonArray json_images;

  // Get all image ids
  auto ids = db_->images().entries().keys();

  // Order ids by their matching images upload date
  std::sort(ids.begin(), ids.end(), [this](Image::ID a, Image::ID b) {
    return this->db_->images().get(a).uploaded() <
           this->db_->images().get(b).uploaded();
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
  json["defaultOctoprint"] = db_->defaultOctoconfig();

  json["profiles"] = json_profiles;

  json["printerTemplate"] = Profile::printerTemplate();
  json["socketTemplate"] = Profile::socketTemplate();
  json["plateTemplate"] = Profile::plateTemplate();
  json["octoprintTemplate"] = Profile::octoprintTemplate();

  return json;
}

QJsonObject APIController::CreateAlgorithmList() {
  QJsonObject json;

  for (Algorithm* algo : colony_detector_->algos())
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
  } catch (Exception const& e) {
    output_->Error("c3picko::exception", e.what(), client);
    qWarning("c3picko::exception %s", e.what());
  } catch (std::exception const& e) {
    output_->Error("std::exception", e.what(), client);
    qWarning("std::exception %s", e.what());
  } catch (...) {
    output_->Error("Exception", "unknown", client);
    qWarning("unknown exception");
  }
}

Database& APIController::db() const { return *db_; }
}  // namespace c3picko
