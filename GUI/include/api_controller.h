#pragma once

#include "database.hpp"
#include "httprequesthandler.h"
#include "include/api_input.h"
#include "include/api_output.h"
#include "include/global.h"
#include "include/types/job.hpp"
#include <QList>
#include <QPointer>
#include <QSignalMapper>
#include <QString>

class QThreadPool;
class ColonyDetector;
namespace c3picko {
namespace pi {
class Command;
}
class APIController : public QObject {
  Q_OBJECT
  friend class APIInput;
  friend class APIOutput;

public:
  APIController(QThreadPool *pool, Database *db, QObject *parent);

  // Forwarded to APIInput
  void request(QJsonObject request, QString raw_request, QObject *client);

signals:
  /**
   * @brief Request to send data to client
   */
  void toClient(QObject *client, QString type, QJsonObject data);
  /**
   * @brief Request to send data to all clients
   */
  void toAll(QString type, QJsonObject data);
  /**
   * @brief Request to send data to all clients except client
   */
  void toAllExClient(QObject *client, QString type, QJsonObject data);
  /**
   * @brief Request to send command to octoprint
   */
  void toPrinter(pi::Command *command);

protected:
  Database &db() const;
  QThreadPool *pool() const;

public slots:
  void DeleteImage(Image::ID, QObject *client);
  void DeleteJob(Job::ID id, QObject *client);
  void UploadImage(Image &, QObject *client);
  void cropImage(Image::ID id, int x, int y, int w, int h,
                 QObject *client); // TODO make rect from (x,y,w,h)
  void createSettingsProfile(Profile &prof_wo_id, QObject *client);
  void updateSettingsProfile(Profile &profile, QObject *client);
  void deleteSettingsProfile(Profile::ID id, QObject *client);
  void createJob(Job &job_wo_id, QObject *client);

  void getPositions(Image::ID id, void *algorithm, void *algo_settings,
                    QObject *client);
  void startJob(Job::ID id, QObject *client);

  void shutdown(QObject *client);
  void restart(QObject *client);

private slots:
  void defaultSignalHandler();

signals:
  void OnUnknownRequest(QString request, QObject *client);

  void OnImageListRequested(QObject *client);
  void OnJobListRequested(QObject *client);
  void OnProfileListRequested(QObject *client);
  void OnAlgorithmListRequested(QObject *client);

  void OnJobCreated(Job, QObject *client);
  void OnJobCreateError(QString, QObject *);
  void OnJobDeleted(Job, QObject *client);
  void OnJobDeleteError(QString path, QObject *client);

  void OnImageCreated(Image, QObject *client);
  void OnImageCreateError(QString path, QObject *client);
  void OnImageDeleted(Image, QObject *client);
  void OnImageDeleteError(QString path, QObject *client);
  void OnImageCropped(Image, QObject *client);
  void OnImageCropError(QString id, QObject *client);

  void OnProfileCreated(Profile profile, QObject *client);
  // not used
  // void OnProfileCreateError(Profile::ID profile, QObject* client);
  void OnProfileUpdated(Profile, QObject *client);
  void OnProfileUpdateError(Profile::ID, QObject *client);
  void OnProfileDeleted(Profile::ID profile, QObject *client);
  void OnProfileDeleteError(Profile::ID profile, QObject *client);

  /**
   * @brief OnColonyDetected
   * @param detector Needs to be deleted
   * @param client
   */
  void OnColonyDetected(ColonyDetector *detector, QObject *client);
  void OnColonyDetectionError(QString error, QObject *client);

private:
  QJsonObject createImageList() const;
  QJsonObject createImageList(Image); // TODO const
  QJsonObject createJobList();
  QJsonObject createJobList(Job);
  QJsonObject createDeleteImage(Image);
  QJsonObject createDeleteJob(Job job);
  QJsonObject createProfileList();
  QJsonObject CreateAlgorithmList();

protected:
  QThreadPool *pool_;
  Database *db_;

  // Deleted by this
  APIInput *input_;
  APIOutput *output_;
};
} // namespace c3picko
