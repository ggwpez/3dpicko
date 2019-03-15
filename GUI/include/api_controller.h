#pragma once

#include <QList>
#include <QPointer>
#include <QSignalMapper>
#include <QString>
#include "database.hpp"
#include "httprequesthandler.h"
#include "include/api_input.h"
#include "include/api_output.h"
#include "include/global.h"
#include "include/types/job.hpp"

class QThreadPool;
class ColonyDetector;
namespace c3picko {
class AlgorithmJob;
namespace pi {
class Command;
}
class APIController : public QObject {
  Q_OBJECT
  friend class APIInput;
  friend class APIOutput;

 public:
  APIController(AlgorithmManager* colony_detector,
                AlgorithmManager* plate_detector, Database* db,
                QObject* parent);

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
  void UploadImage(Image&, QObject* client);
  void cropImage(Image::ID id, int x, int y, int w, int h,
                 QObject* client);  // TODO make rect from (x,y,w,h)
  void createSettingsProfile(Profile& prof_wo_id, QObject* client);
  void updateSettingsProfile(Profile& profile, QObject* client);
  void deleteSettingsProfile(Profile::ID id, QObject* client);
  void setDefaultSettingsProfile(Profile::ID id, QObject* client);
  void setStartingWell(Job::ID id, Profile::ID plate_id, int row, int col,
                       QObject* client);

  /**
   * @brief createJob
   * @param job_wo_id Job without a valid id. The id will be set in this
   * function.
   * @param client
   */
  void createJob(Job& job_wo_id, QObject* client);

  /**
   * @brief Calls detectColonies with default parameters.
   */
  void getPositions(Image::ID id, QObject* client);
  /**
   * @brief Calls detectColonies with the parameters given by the client.
   */
  void updateDetectionSettings(Job::ID job, QString algo_id,
                               QJsonObject settings, QObject* client);
  void startJob(Job::ID id, QObject* client);

  void shutdown(QObject* client);
  void restart(QObject* client);

 private slots:
  void defaultSignalHandler();

 signals:
  void OnUnknownRequest(QString request, QObject* client);

  void OnImageListRequested(QObject* client);
  void OnJobListRequested(QObject* client);
  void OnProfileListRequested(QObject* client);
  void OnAlgorithmListRequested(QObject* client);

  void OnJobCreated(Job, QObject* client);
  void OnJobCreateError(QString, QObject*);
  void OnJobDeleted(Job, QObject* client);
  void OnJobDeleteError(QString path, QObject* client);

  void OnImageCreated(Image, QObject* client);
  void OnImageCreateError(QString path, QObject* client);
  void OnImageDeleted(Image, QObject* client);
  void OnImageDeleteError(QString path, QObject* client);
  void OnImageCropped(Image, QObject* client);
  void OnImageCropError(QString id, QObject* client);

  void OnProfileCreated(Profile profile, QObject* client);
  // not used
  // void OnProfileCreateError(Profile::ID profile, QObject* client);
  void OnProfileUpdated(Profile, QObject* client);
  void OnProfileUpdateError(Profile::ID, QObject* client);
  void OnProfileDeleted(Profile::ID profile, QObject* client);
  void OnProfileDeleteError(Profile::ID profile, QObject* client);
  void OnDefaultSettingsProfileSet(Profile::ID profile, QObject* client);
  void OnDefaultSettingsProfileSetError(QString error, QObject* client);

  void OnSetStartingWell(Job::ID job, Profile::ID plate, int row, int col,
                         QObject* client);
  void OnSetStartingWellError(QString error, QObject* client);

  void OnColonyDetectionStarted(Job::ID, QObject* client);
  /**
   * @brief OnColonyDetected
   * @param colonies Needs to be deleted. Normally done in
   * APIOutput::ColonyDetected
   * @param client
   */
  void OnColonyDetected(std::vector<Colony> const* colonies, QObject* client);
  void OnColonyDetectionError(QString error, QObject* client);

 private:
  QJsonObject createImageList() const;
  QJsonObject createImageList(Image);  // TODO const
  QJsonObject createJobList();
  QJsonObject createJobList(Job);
  QJsonObject createDeleteImage(Image);
  QJsonObject createDeleteJob(Job job);
  QJsonObject createProfileList();
  QJsonObject CreateAlgorithmList();

  AlgorithmJob* detectColonies(Job::ID job_id, QString algo_id,
                               QJsonObject settings, QObject* client);

 protected:
  AlgorithmManager *colony_detector_, *plate_detector_;
  Database* db_;

  // Deleted by this
  APIInput* input_;
  APIOutput* output_;
};
}  // namespace c3picko
