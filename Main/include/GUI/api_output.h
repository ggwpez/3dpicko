#pragma once

#include <QObject>
#include <QString>

#include "GUI/types/job.h"
#include "GUI/types/report.h"
#include "Main/global.h"
#include "Main/version.h"

namespace c3picko {
class Colony;
class AlgorithmManager;
class APIInput;
class APIController;

/**
 * @brief Creates the messages that should be send to the client(s) as response
 * for the given event. All these SLOTS map to the according SIGNALS from the
 * APIController op_. Emits the to* SIGNALS from the APIController member op_.
 */
class APIOutput : public QObject {
  Q_OBJECT

  friend class APIInput;
  friend class APIController;

 public:
  APIOutput(APIController* parent);

 protected slots:
  void UnknownRequest(QString request, QObject* client);

  void VersionListRequested(QObject* client);
  void ImageListRequested(QObject* client);
  void JobListRequested(QObject* client);
  void ProfileListRequested(QObject* client);
  void AlgorithmListRequested(QObject* client);

  void JobCreated(Job::ID, QObject* client);
  void JobCreateError(QString, QObject*);
  void JobDeleted(Job::ID id, QObject* client);
  void JobDeleteError(Job::ID id, QObject* client);
  void JobStarted(Job::ID id, QObject* client);
  void JobStartError(QString error, QObject* client);

  void Report(Job::ID id, QString path, QObject* client);
  void ReportError(QString error, QObject* client);

  void ImageCreated(Image::ID, QObject* client);
  void ImageCreateError(QString path, QObject* client);
  void ImageDeleted(Image::ID id, QObject* client);
  void ImageDeleteError(QString path, QObject* client);

  void ProfileCreated(Profile profile, QObject* client);
  void ProfileCreateError(QString error, QObject* client);
  void ProfileUpdated(Profile, QObject* client);
  void ProfileUpdateError(Profile::ID, QObject* client);
  void ProfileDeleted(Profile::ID profile, QObject* client);
  void ProfileDeleteError(Profile::ID profile, QObject* client);

  void DefaultSettingsProfileSet(Profile::ID profile, QObject* client);
  void DefaultSettingsProfileSetError(QString error, QObject* client);

  void SetStartingWell(Job::ID job, Profile::ID plate, int row, int col,
					   QObject* client);
  void SetStartingWellError(QString error, QObject* client);

  void SetColoniesToPick(Job::ID job, QSet<Colony::ID> colonies,
						 QObject* client);
  void SetColoniesToPickError(QString error, QObject* client);

  void ColonyDetectionStarted(Job::ID, QObject* client);

  void UpdateRequested(QObject* client);
  /**
   * @brief Always call this from ColonyDetector or derived!
   */
  void ColonyDetected(Job::ID job, std::vector<Colony> const* detector,
					  QObject* client);
  /**
   * @brief Always call this from ColonyDetector or derived!
   */
  void ColonyDetectionError(QString error, QObject* client);

  void VersionSwitched(Version::ID);

  void Error(QString where, QString what, QObject* client);

 private:
  // TODO change name
  APIController* op;
};
}  // namespace c3picko
