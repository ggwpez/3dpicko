#include "include/api_controller.h"
#include "include/algorithm_pipeline.h"
#include "include/gcodegenerator.h"

#include "include/algo1_test.h"
#include "include/algo_setting.h"

#include "include/commands/arbitrary_command.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMetaMethod>
#include <QThreadPool>

using namespace c3picko::pi;
namespace c3picko {
APIController::APIController(AlgorithmPipeline *detector, Database *db,
                             QObject *parent)
    : QObject(parent), detector_(detector), db_(db), input_(new APIInput(this)),
      output_(new APIOutput(this)) {
  QMetaObject const *meta = this->metaObject();
  QMetaMethod default_handler =
      meta->method(meta->indexOfSlot(qPrintable("defaultSignalHandler()")));

  // TODO use QMetaObject::connectSlotsByName ?
  for (int i = meta->methodOffset(); i < meta->methodCount(); ++i) {
    QMetaMethod signal = meta->method(i);
    QString slot_name =
        QMetaObject::normalizedSignature(qPrintable(signal.methodSignature()));

    if (signal.methodType() == QMetaMethod::Signal) {
      if (!slot_name.startsWith("On"))
        continue;
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
  QMetaObject const *meta_sender = QObject::sender()->metaObject();
  QString signal_name = QMetaObject::normalizedSignature(
      meta_sender->method(QObject::senderSignalIndex()).name());

  qDebug("Event %s::%s", qPrintable(meta_sender->className()),
         qPrintable(signal_name));
}

void APIController::DeleteImage(Image::ID id, QObject *client) {
  // Does the image exist?
  if (!db_->images().exists(id)) {
    emit OnImageDeleteError(id, client);
    return;
  }
  // Is the image	used by a job?
  for (auto const &job : db_->jobs()) {
    if (job.img_id() == id) {
      emit OnImageDeleteError(id, client);
      return;
    }
  }

  Image image = db_->images().get(id);
  image.clearCache();

  if (image.deleteFile()) {
    db_->deletedImages().add(id, image);
    db_->images().remove(
        id); // Carefull here, if we use a reference to image instead, it will
             // go out of scope after deletion
    emit OnImageDeleted(image, client);
  } else {
    emit OnImageDeleteError(image.path(), client);
  }
}

void APIController::DeleteJob(Job::ID id, QObject *client) {
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

void APIController::UploadImage(Image &image, QObject *client) {
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
                              QObject *client) {
  if (!db_->images().exists(id))
    return emit OnImageCropError(id, client);
  else {
    QString error;
    Image cropped;
    Image &original = db_->images().get(id);

    // Is the cropped image empty?
    if (!original.crop(x, y, w, h, cropped, error)) {
      emit OnImageCropError(id, client); // TODO inform client
      return;
    }
    if (!cropped.writeToFile()) // save cropped image to the hdd
    {
      emit OnImageCreateError("<cropped>", client);
      return;
    }

    db_->images().add(cropped.id(), cropped);
    // response = {{"id", cropped.id()}}; // TODO
    emit OnImageCropped(cropped, client);
  }
}

void APIController::createSettingsProfile(Profile &profile_wo_id,
                                          QObject *client) {
  Profile::ID id = db_->newProfileId();
  profile_wo_id.setId(id);

  db_->profiles().add(id, profile_wo_id);
  // profile.write(response);
  emit OnProfileCreated(profile_wo_id, client);
}

void APIController::updateSettingsProfile(Profile &profile, QObject *client) {
  if (!db_->profiles().exists(profile.id())) {
    // response = {{"error", "Profile Id unknown: '" +profile.id() +"'"}};
    emit OnProfileUpdateError(profile.id(), client);
  } else {
    db_->profiles().add(profile.id(), profile);
    // response = json_profile;
    emit OnProfileUpdated(profile, client);
  }
}

void APIController::deleteSettingsProfile(Profile::ID id, QObject *client) {
  if (!db_->profiles().exists(id)) {
    emit OnProfileDeleteError(id, client);
  } else {
    // FIXME cant delete profiles used by jobs
    emit OnProfileDeleted(id, client);
    db_->profiles().remove(id);
  }
}

void APIController::createJob(Job &job, QObject *client) {
  Job::ID id = db_->newJobId();
  job.setCreationDate(QDateTime::currentDateTime());

  if (!db_->profiles().exists(job.printer()))
    emit OnJobCreateError("Printer profile " + id + " unknown", client);
  else if (!db_->profiles().exists(job.socket()))
    emit OnJobCreateError("Socket profile " + id + " unknown", client);
  else {
    job.setId(id);
    db_->jobs().add(id, job);
    emit OnJobCreated(job, client);
  }
}

void APIController::getPositions(Image::ID id, void *algorithm,
                                 void *algo_settings, QObject *client) {
  if (!db_->images().exists(id)) {
    qWarning() << "Colony detection error: image not found (#" << id << ")";
    emit OnColonyDetectionError("Image not found", client);
  } else {
    Image &img =
        db_->images().get(id); // Non const& bc readCvMat() can set the cache
    cv::Mat data;

    if (!img.readCvMat(data)) {
      qCritical() << "CV could not read image" << img.path();
      emit OnColonyDetectionError("Image not readable or empty", client);
      return;
    }

    /*		AlgorithmPipeline* detector = new AlgorithmPipeline(data.clone(), new
Algo1Test()); // Clone to avoid thread problems TODO
       debug
                    // Set the connection context to client, so in case it
disconnects we
                    // dont leak memory
                    connect(detector, &AlgorithmPipeline::OnFinished, client,
                                    [this, client, detector](qint64 ms) {
                                            qDebug() << "Colonies detected in"
<< ms << "ms";
                                            emit
this->OnColonyDetected(detector, client);
                                    },
                                    Qt::QueuedConnection);
                    connect(detector, SIGNAL(OnFinished(qint64)), detector,
SLOT(deleteLater()));
FIXME
                    detector->setAutoDelete(false);
                    pool_->start(detector);*/
  }
}

void APIController::updateDetectionSettings(Job::ID job_id, QString algo_id,
                                            QJsonObject settings,
                                            QObject *client) {
  if (!db_->jobs().exists(job_id)) {
    qWarning() << "Client requested colony detection for unknown job, id="
               << job_id;
    emit OnColonyDetectionError("Job not found", client);
  } else {
    Image::ID img_id = db_->jobs().get(job_id).img_id();

    if (!db_->images().exists(img_id)) {
      qWarning() << "Colony detection error: image not found (#" << img_id
                 << ")";
      emit OnColonyDetectionError("Image not found", client);
    } else {
      Image &img = db_->images().get(
          img_id); // Non const& bc readCvMat() can set the cache
      cv::Mat data;

      if (!img.readCvMat(data)) {
        qCritical() << "CV could not read image" << img.path();
        emit OnColonyDetectionError("Image not readable or empty", client);
      } else {
        AlgorithmJob *job = detector_->createJob(data, algo_id, settings);

        if (!job) {
          emit OnColonyDetectionError("Algorithm not found", client);
        } else {
          connect(job, &AlgorithmJob::OnFinished, client, [this, client](
                                                              void *output) {
            emit this->OnColonyDetected(
                reinterpret_cast<std::vector<Colony> *>(output), client);
          });
          job->start(true, true);
        }
      }
    }
  }
}

void APIController::startJob(Job::ID id, QObject *client) {
  /*PrinterProfile*		printerp =
  (PrinterProfile*)(db_->profiles().get("302"));
  PlateSocketProfile* socket   =
  (PlateSocketProfile*)(db_->profiles().get("303"));
  PlateProfile*		plate	= (PlateProfile*)(db_->profiles().get("305"));

  GcodeGenerator gen(*socket, *printerp, *plate);

  std::vector<LocalColonyCoordinates> coords;
  for (int x = 0; x < 4; ++x)
                  for (int y = 0; y < 5; ++y)
                                  coords.push_back(Point(10 * x + 70, y * 10 +
  20));

  auto			   code = gen.CreateGcodeForTheEntirePickingProcess(1,
  8, coords);
  std::ostringstream s;

  QStringList sum;
  for (auto c : code)
                  sum << QString::fromStdString(c.ToString());

  Command* cmd = commands::ArbitraryCommand::MultiCommand(sum);

  QObject::connect(cmd, &Command::OnStatusOk, &OnStatusOk);
  QObject::connect(cmd, &Command::OnStatusErr, &OnStatusErr);
  QObject::connect(cmd, &Command::OnNetworkErr, &OnNetworkErr);

  QObject::connect(cmd, SIGNAL(OnFinished()), cmd, SLOT(deleteLater()));

  printer.SendCommand(cmd);*/
}

void APIController::shutdown(QObject *client) {
  qDebug() << "Client requested shutdown at"
           << QDateTime::currentDateTime().toString();
  qApp->exit(0);
}

void APIController::restart(QObject *client) {
  qDebug() << "Client requested restart at"
           << QDateTime::currentDateTime().toString();
  qApp->exit(123);
}

QJsonObject APIController::createImageList() const {
  QJsonArray image_list;
  for (auto const &image : db_->images()) {
    QJsonObject json;

    image.write(json);
    image_list.push_back(json);
  }

  return {{"images", image_list}};
}

QJsonObject APIController::createImageList(Image img) {
  QJsonArray json_jobs;
  QJsonObject json_job;

  img.write(json_job);
  json_jobs.append(json_job);

  return {{"images", json_jobs}};
}

QJsonObject APIController::createJobList() {
  QJsonArray json_jobs;

  for (auto const &job : db_->jobs()) {
    QJsonObject json;
    job.write(json);
    json_jobs.append(json);
  }

  return {{"jobs", json_jobs}};
}

QJsonObject APIController::createJobList(Job job) {
  QJsonArray json_jobs;
  QJsonObject json_job;

  job.write(json_job);
  json_jobs.append(json_job);

  return {{"jobs", json_jobs}};
}

QJsonObject APIController::createProfileList() {
  QJsonArray json_jobs;

  for (auto const &profile : db_->profiles()) {
    QJsonObject json;
    profile.write(json);
    json_jobs.append(json);
  }

  return {{"profiles", json_jobs}};
}

QJsonObject APIController::CreateAlgorithmList() {
  QJsonObject json;

  for (Algorithm *algo : detector_->algos())
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
                            QObject *client) {
  try {
    input_->serviceRequest(request, raw_request, client);
  } catch (std::exception const &e) {
    output_->Error("Exception", e.what(), client);
    qWarning("std::exception %s", e.what());
  } catch (...) {
    output_->Error("Exception", "unknown", client);
    qWarning("unknown exception");
  }
}

Database &APIController::db() const { return *db_; }
} // namespace c3picko
