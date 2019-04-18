#pragma once

#include <QList>
#include <QPointer>
#include <QSignalMapper>
#include <QString>
#include "database.h"
#include "httprequesthandler.h"
#include "include/api_input.h"
#include "include/api_output.h"
#include "include/global.h"
#include "include/types/job.h"

class QThreadPool;
class ColonyDetector;
namespace c3picko {
class VersionManager;
class AlgorithmJob;
namespace pi {
class Command;
}
/**
 * @brief Contains the logic for the API.
 * Works together with APIInput and APIOutput.
 * The public functions are called by APIInput which then process the input and
 * emit the neccecary SIGNALS that are connected to the matching SLOTS from
 * APIOutput. APIOutput then informs the clients of the result from these
 * functions.
 */
class APIController : public QObject {
  Q_OBJECT
  friend class APIInput;
  friend class APIOutput;

 public:
  /**
   * @brief APIController
   * @param colony_detector
   * @param plate_detector
   * @param version_manager Must run in the same thread as this . Argument can
   * be null.
   * @param db
   * @param parent
   */
  APIController(AlgorithmManager* colony_detector,
                AlgorithmManager* plate_detector,
                VersionManager* version_manager, Database* db, QObject* parent);

  // Forwarded to APIInput
  void request(QJsonObject request, QString raw_request, QObject* client);

 signals:
  /**
   * @brief Request to send data to client
   */
  void toClient(QObject* client, QString type, QJsonObject data);
  /**
   * @brief Request to send data to all clients
   */
  void toAll(QString type, QJsonObject data);
  /**
   * @brief Request to send data to all clients except client
   */
  void toAllExClient(QObject* client, QString type, QJsonObject data);
  /**
   * @brief Request to send command to octoprint
   */
  void toPrinter(pi::Command* command);

 protected:
  Database& db() const;
  QThreadPool* pool() const;

 public:
  void DeleteImage(Image::ID, QObject* client);
  void DeleteJob(Job::ID id, QObject* client);
  void UploadImage(Image, QObject* client);
  void createSettingsProfile(Profile& prof_wo_id, QObject* client);
  void updateSettingsProfile(Profile& profile, QObject* client);
  void deleteSettingsProfile(Profile::ID id, QObject* client);
  void setDefaultSettingsProfile(Profile::ID id, QObject* client);
  void setStartingWell(Job::ID id, Profile::ID plate_id, int row, int col,
                       QObject* client);
  void setColoniesToPick(Job::ID id, QSet<Colony::ID> ex_user,
                         QSet<Colony::ID> in_user, quint32 number,
                         QObject* client);

  /**
   * @brief createJob
   * @param job_wo_id Job without a valid id. The id will be set in this
   * function.
   * @param client
   */
  void createJob(Job& job_wo_id, QObject* client);

  /**
   * @brief Calls detectColonies with the parameters given by the client.
   */
  void updateDetectionSettings(Job::ID job, QString algo_id,
                               QJsonObject settings, QObject* client);
  void startJob(Job::ID id, Profile::ID octoprint, QObject* client);

  void shutdown(QObject* client);
  void restart(QObject* client);

 private slots:
  /**
   * @brief Catches and logs all incoming signals.
   */
  void defaultSignalHandler();
  /**
   * @brief VersioManager::OnInstallBegin
   */
  // void versionInstallBegin(Version::ID);
  /**
   * @brief VersioManager::OnInstalled
   */
  // void versionInstalled(Version::ID);
  /**
   * @brief VersioManager::OnInstallError
   */
  // void versionInstallError(Version::ID, QString error);
  /**
   * @brief VersioManager::OnUnInstalled
   */
  // void versionUnInstalled(Version::ID);
  /**
   * @brief VersioManager::OnSwitched
   */
  // void versionSwitched(Version::ID);

 signals:
  void OnUnknownRequest(QString request, QObject* client);

  void OnVersionListRequested(QObject* client);
  void OnImageListRequested(QObject* client);
  void OnJobListRequested(QObject* client);
  void OnProfileListRequested(QObject* client);
  void OnAlgorithmListRequested(QObject* client);

  // Passing job as signal arg should not work, bc Job is not registered as a
  // QMetaType but it still does, as long as all runs in the same thread FIXME
  void OnJobCreated(Job::ID, QObject* client);
  void OnJobCreateError(QString, QObject*);
  void OnJobDeleted(Job::ID, QObject* client);
  void OnJobDeleteError(Job::ID id, QObject* client);

  void OnJobStarted(Report, QObject* client);
  void OnJobStartError(QString error, QObject* client);

  void OnImageCreated(Image::ID, QObject* client);
  void OnImageCreateError(QString path, QObject* client);
  void OnImageDeleted(Image::ID, QObject* client);
  void OnImageDeleteError(QString path, QObject* client);

  void OnProfileCreated(Profile profile, QObject* client);
  void OnProfileCreateError(QString error, QObject* client);
  void OnProfileUpdated(Profile, QObject* client);
  void OnProfileUpdateError(Profile::ID, QObject* client);
  void OnProfileDeleted(Profile::ID profile, QObject* client);
  void OnProfileDeleteError(Profile::ID profile, QObject* client);
  void OnDefaultSettingsProfileSet(Profile::ID profile, QObject* client);
  void OnDefaultSettingsProfileSetError(QString error, QObject* client);

  void OnSetStartingWell(Job::ID job, Profile::ID plate, int row, int col,
                         QObject* client);
  void OnSetStartingWellError(QString error, QObject* client);

  void OnSetColoniesToPick(Job::ID job, QSet<Colony::ID> colonies,
                           QObject* client);
  void OnSetColoniesToPickError(QString error, QObject* client);

  void OnColonyDetectionStarted(Job::ID, QObject* client);
  /**
   * @brief OnColonyDetected
   * @param colonies Needs to be deleted. Normally done in
   * APIOutput::ColonyDetected
   * @param client
   */
  void OnColonyDetected(Job::ID job, std::vector<Colony> const* colonies,
                        QObject* client);
  void OnColonyDetectionError(QString error, QObject* client);

  void OnVersionSwitched(Version::ID);

 private:
  QJsonObject createVersionList() const;
  QJsonObject createImageList() const;
  static QJsonObject createImageList(Image const&);
  QJsonObject createJobList();
  QJsonObject createJobList(Job const&);
  QJsonObject createDeleteImage(const Image&);
  QJsonObject createDeleteJob(const Job& job);
  QJsonObject createProfileList();
  QJsonObject CreateAlgorithmList();

  std::shared_ptr<AlgorithmJob> detectColonies(Job::ID job_id, QString algo_id,
                                               QJsonObject settings,
                                               QObject* client);

 protected:
  AlgorithmManager *colony_detector_, *plate_detector_;
  VersionManager* version_manager_;
  Database* db_;

  // Deleted by this
  APIInput* input_;
  APIOutput* output_;
};
}  // namespace c3picko
