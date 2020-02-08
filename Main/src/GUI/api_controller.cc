#include "GUI/api_controller.h"
#include "GUI/reporter.h"
#include "GUI/types/well.h"
#include "Gcode/gcodegenerator.h"
#include "ImageRecognition/algorithm_job.h"
#include "ImageRecognition/algorithm_manager.h"
#include "ImageRecognition/algorithm_result.h"
#include "ImageRecognition/detection_result.h"
#include "ImageRecognition/plate_result.h"
#include "Main/exception.h"
#include "Main/version_manager.h"
#include "PiCommunicator/commands/arbitrary_command.h"
#include "PiCommunicator/octoprint.h"

#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QMetaMethod>
#include <functional>
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
  for (auto const& version : db_->versions())
	all.push_back(Marshalling::toJson(version.second));

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
  if (!db_->images().exists(id)) return emit OnImageDeleteError(id, client);
  // Is the image	not used by a job?
  for (auto const& pair : db_->jobs()) {
	if (pair.second.imgID() == id) return emit OnImageDeleteError(id, client);
  }

  Image& image = db_->images().get(id);
  image.clearCache();

  if (image.deleteFile()) {
	db_->images().remove(id);
	// Image& invalid since here

	if (db_->detectedPlates().exists(id)) db_->detectedPlates().remove(id);

	emit OnImageDeleted(id, client);
  } else
	emit OnImageDeleteError(image.path().toSystemAbsolute(), client);
}

void APIController::DeleteJob(Job::ID id, QObject* client) {
  if (!db_->jobs().exists(id))
	emit OnJobDeleteError(id, client);
  else {
	db_->jobs().remove(id);
	emit OnJobDeleted(id, client);
  }
}

void APIController::UploadImage(Image image, QObject* client) {
  std::shared_ptr<cv::Mat> raw(std::make_shared<cv::Mat>());
  if (!image.readCvMat(*raw))
	return emit OnImageCreateError("Could not read/find image", client);

  PlateProfile* plate(db_->profiles().get(db_->defaultPlate()));
  Algorithm::ID aid(toString(plate->plateType()));
  AlgorithmJob::ID jid = db_->newResultJobId();
  std::shared_ptr<PlateResult> result(
	  std::make_shared<PlateResult>(db_->newResultId()));

  AlgorithmJob* ajob =
	  plate_detector_->createJob(aid, jid, result, QJsonObject());
  ajob->pushInput(raw.get());

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
			  else {
				db_->images().add(cropped.id(), cropped);
				db_->detectedPlates().add(cropped.id(),
										  std::move(result->rotated()));
			  }

			  emit this->OnImageCreated(cropped.id(), client);
			  qDebug() << "Detected Plate in" << ajob->tookMs() << "ms";
			});
	connect(ajob, &AlgorithmJob::OnAlgoFailed, ajob, [this, ajob, client] {
	  emit OnImageCreateError(ajob->result()->stageError(), client);
	});
	connect(ajob, &AlgorithmJob::OnFinished, ajob, [raw] {});

	ajob->start(true, true);
  }
}

void APIController::updateSoftware(QObject* client) {
  emit OnUpdateRequested(client);
}

void APIController::createSettingsProfile(Profile& profile_wo_id,
										  QObject* client) {
  Profile::ID id = db_->newProfileId();
  profile_wo_id.setId(id);

  // check that there is not already an octoprint profile
  if (profile_wo_id.type() == ProfileType::OCTOPRINT) {
	for (auto it = db_->profiles().begin(); it != db_->profiles().end(); ++it)
	  if (it->second.type() == ProfileType::OCTOPRINT)
		return emit OnProfileCreateError(
			"Cant create more than one octoprint profile", client);
  }

  db_->profiles().add(id, profile_wo_id);
  emit OnProfileCreated(profile_wo_id, client);
}

void APIController::updateSettingsProfile(Profile& profile, QObject* client) {
  if (!db_->profiles().exists(profile.id()))
	emit OnProfileUpdateError(profile.id(), client);
  else {
	db_->profiles().add(profile.id(), profile);
	emit OnProfileUpdated(profile, client);
  }
}

void APIController::deleteSettingsProfile(Profile::ID id, QObject* client) {
  if (!db_->profiles().exists(id))
	emit OnProfileDeleteError(id, client);
  else if (isProfileUsedByJob(id))
	emit OnProfileDeleteError("used by a job", client);
  else if (isProfileDefault(id))
	emit OnProfileDeleteError("is default", client);
  else {
	emit OnProfileDeleted(id, client);
	db_->profiles().remove(id);
  }
}

void APIController::setDefaultSettingsProfile(Profile::ID id, QObject* client) {
  if (!db_->profiles().exists(id))
	emit OnDefaultSettingsProfileSetError("Profile " + id + " not found",
										  client);
  else {
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
	  return emit OnDefaultSettingsProfileSetError("Database error", client);
	}

	emit OnDefaultSettingsProfileSet(id, client);
  }
}

void APIController::setStartingWell(Job::ID id, Profile::ID plate_id, int row,
									int col, QObject* client) {
  if (!db_->jobs().exists(id))
	emit OnSetStartingWellError("Job " + id + " not found", client);
  else if (!db_->profiles().exists(plate_id))
	emit OnSetStartingWellError("Plate " + plate_id + " not found", client);
  else {
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
  job.setOctoprint(db_->defaultOctoconfig());

  if (!db_->profiles().exists(job.printer()))
	emit OnJobCreateError("Printer profile " + id + " unknown", client);
  else if (!db_->profiles().exists(job.socket()))
	emit OnJobCreateError("Socket profile " + id + " unknown", client);
  else if (!db_->profiles().exists(job.octoprint()))
	emit OnJobCreateError("Octoprint profile " + id + " unknown", client);
  else {
	job.setId(id);
	db_->jobs().add(id, job);
	emit OnJobCreated(job.id(), client);
  }
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
	  std::shared_ptr<cv::Mat> image(std::make_shared<cv::Mat>());

	  if (!img.readCvMat(*image)) {
		qCritical() << "CV could not read image"
					<< img.path().toSystemAbsolute();
		emit OnColonyDetectionError("Image not readable or empty", client);
	  } else {
		AlgorithmResult::ID result_id = db_->newResultId();
		AlgorithmJob::ID algo_job_id = db_->newResultJobId();
		std::shared_ptr<DetectionResult> result(
			std::make_shared<DetectionResult>(result_id));
		std::shared_ptr<AlgorithmJob> algo_job(colony_detector_->createJob(
			algo_id, algo_job_id, result, settings));
		AlgorithmJob* raw = algo_job.get();

		algo_job->pushInput(image.get());
		if (db_->detectedPlates().exists(img_id))
		  // Nobody can delete an image while the job uses it, so the plate also
		  // wont be deleted
		  algo_job->pushInput(db_->detectedPlates().get(img_id).get());

		if (!algo_job) {
		  emit OnColonyDetectionError("Algorithm not found", client);
		} else {
		  job.setResultAlgorithmJob(algo_job);
		  connect(raw, &AlgorithmJob::OnAlgoSucceeded, raw,
				  [this, client, job_id, raw, result] {
					emit this->OnColonyDetected(job_id, &result->colonies(),
												client);
					qDebug() << "Detected" << result->colonies().size()
							 << "colonies in" << raw->tookMs() << "ms";
				  });
		  connect(raw, &AlgorithmJob::OnAlgoFailed, raw, [this, raw, client] {
			emit OnColonyDetectionError(raw->result()->stageError(), client);
		  });
		  // Keep the shared pointer until the job is done. I doubt that this is
		  // good style...
		  connect(raw, &AlgorithmJob::OnFinished, raw, [image] {});

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

QString APIController::calculateGcode(
	Job::ID id, std::vector<GcodeInstruction>* gcode,
	std::map<Well, Colony::ID>* pick_positions) {
  if (!db_->jobs().exists(id)) return "Job '" + id + "' job found";
  Job const& job = db_->jobs().get(id);

  if (!job.resultJob() || !job.resultJob()->result())
	return "Job " + id +
		   " could not find its detection results: " + job.resultID();
  DetectionResult* result =
	  static_cast<DetectionResult*>(job.resultJob()->result().get());

  if (!db_->profiles().exists(job.printer()) ||
	  !db_->profiles().exists(job.socket()) ||
	  !db_->profiles().exists(job.plate()) ||
	  !db_->profiles().exists(job.octoprint()))
	return "Internal error: Cant find printer, socket, plate or octoprint "
		   "profile";

  PrinterProfile* printerp(db_->profiles().get(job.printer()));
  PlateSocketProfile* socket(db_->profiles().get(job.socket()));
  PlateProfile* plate(db_->profiles().get(job.plate()));
  Plate* detectedPlate(db_->detectedPlates().get(job.imgID()).get());
  Image const& image(db_->images().get(job.imgID()));

  GcodeGenerator gen(*socket, *printerp, *plate);

  // Order the colonies by their top left position
  QSet<Colony::ID> selected = job.coloniesToPick();

  std::vector<LocalColonyCoordinates> coords;
  Well well(job.startingRow(), job.startingCol(), plate);
  // Convert the colony coordinates to real world coordinates
  {
	for (auto it = selected.begin(); it != selected.end(); ++it) {
	  auto colony =
		  std::find_if(result->includedBegin(), result->includedEnd(),
					   [&it](Colony const& c) { return c.id() == *it; });

	  if (colony == result->includedEnd())
		return "Internal error: Cant find selected colony";

	  // Assert that they are within the inner border
	  if (!detectedPlate->isPixelPickable(colony->x() * image.width(),
										  colony->y() * image.height())) {
		qWarning() << "cant not pick detected colony (id=" << colony->id()
				   << ")";
		continue;
	  }

	  // Map the image coordinate
	  coords.push_back(
		  detectedPlate->mapImageToGlobal(colony->x(), colony->y()));
	  pick_positions->emplace(well, *it);
	  if (it + 1 != selected.end()) ++well;
	}
  }

  *gcode = gen.CreateGcodeForTheEntirePickingProcess(job.startingRow(),
													 job.startingCol(), coords);
  return "";
}

void APIController::getReport(Job::ID id, QObject* client) {
  std::vector<GcodeInstruction> gcode;
  std::map<Well, Colony::ID> pick_positions;
  auto error = calculateGcode(id, &gcode, &pick_positions);
  if (!error.isEmpty()) return emit OnReportError(error, client);

  Reporter reporter(Reporter::fromDatabase(*db_, id, pick_positions, gcode));
  Report report = reporter.createReport();
  db_->reports().add(id, report);

  emit OnReport(id, report.data().toDocRootAbsolute(), client);
}

void APIController::startJob(Job::ID id, QObject* client) {
  std::vector<GcodeInstruction> gcode;
  std::map<Well, Colony::ID> pick_positions;
  auto error = calculateGcode(id, &gcode, &pick_positions);
  if (!error.isEmpty())
	return emit OnJobStartError("calculating gcode:" + error, client);

  Job const& job = db_->jobs().get(id);
  // Convert gcode to octoprint commands
  QStringList gcode_list;
  for (auto const& c : gcode)
	gcode_list << QString::fromStdString(c.ToString());

  OctoConfig* octoprint(db_->profiles().get(job.octoprint()));
  OctoPrint* printer = new OctoPrint(*octoprint, this);
  Command* cmd = commands::ArbitraryCommand::MultiCommand(gcode_list);
  connect(cmd, &Command::OnStatusErr, [this, client](QJsonValue status) {
	emit OnJobStartError("Octoprint error: " + status.toString(), client);
  });
  connect(cmd, &Command::OnNetworkErr, [this, client](QString error) {
	emit OnJobStartError("Network error: " + error, client);
  });
  connect(cmd, &Command::OnStatusOk,
		  [this, id, client] { emit OnJobStarted(id, client); });
  connect(cmd, &Command::OnFinished, printer, &OctoPrint::deleteLater);
  printer->SendCommand(cmd);
}

void APIController::shutdown(QObject*) {
  qDebug() << "Shutdown";
  qApp->exit(exitCodeSuccess());
}

void APIController::restart(QObject*) {
  qDebug() << "Restart";
  qApp->exit(exitCodeSoftRestart());
}

QJsonObject APIController::createImageList() const {
  std::map<int, int> l;

  // Order Images by upload date
  std::vector<std::reference_wrapper<Image const>> pairs;
  for (auto const& pair : db_->images())
	pairs.push_back(
		std::cref<Image>(pair.second));  // Copy here bc reference_wrapper does
										 // not have move ctor ?!
  std::sort(pairs.begin(), pairs.end(),
			[](std::reference_wrapper<Image const> a,
			   std::reference_wrapper<Image const> b) {
			  return a.get().uploaded() < b.get().uploaded();
			});

  QJsonArray json_images;

  for (auto const& pair : pairs)
	json_images.push_back(Marshalling::toJson(pair.get()));

  return {{"images", json_images}};
}

QJsonObject APIController::createImageList(Image const& img) {
  QJsonArray json_images;

  json_images.append(Marshalling::toJson(img));

  return {{"images", json_images}};
}

QJsonObject APIController::createJobList() {
  QJsonArray json_jobs;

  for (auto const& pair : db_->jobs())
	json_jobs.append(Marshalling::toJson(pair.second));

  return {{"jobs", json_jobs}};
}

QJsonObject APIController::createJobList(const Job& job) {
  QJsonArray json_jobs;

  json_jobs.append(Marshalling::toJson(job));

  return {{"jobs", json_jobs}};
}

QJsonObject APIController::createProfileList() {
  QJsonObject json;
  QJsonArray json_profiles;

  for (auto const& pair : db_->profiles())
	json_profiles.append(Marshalling::toJson(pair.second));

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

QJsonObject APIController::createDeleteImage(Image const& img) {
  return {{"id", img.id()}};
}

QJsonObject APIController::createDeleteJob(Job const& job) {
  return {{"id", job.id()}};
}

bool APIController::isProfileUsedByJob(Profile::ID profile) {
  for (auto const& pair : db_->jobs()) {
	Job const& job(pair.second);
	if (job.plate() == profile || job.printer() == profile ||
		job.socket() == profile || job.octoprint() == profile)
	  return true;
  }

  return false;
}

bool APIController::isProfileDefault(Profile::ID profile) {
  return (db_->defaultPlate() == profile || db_->defaultSocket() == profile ||
		  db_->defaultPrinter() == profile ||
		  db_->defaultOctoconfig() == profile);
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
