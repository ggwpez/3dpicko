#pragma once

#include <QString>
#include "database.hpp"
#include "include/global.h"
#include "include/types/job.hpp"

#include <QObject>

namespace c3picko {
class Colony;
class AlgorithmManager;
class APIInput;
class APIController;
class APIOutput : public QObject {
  Q_OBJECT

  friend class APIInput;
  friend class APIController;

 public:
  APIOutput(APIController* parent);

 protected slots:
  void UnknownRequest(QString request, QObject* client);
  void ImageListRequested(QObject* client);
  void JobListRequested(QObject* client);
  void ProfileListRequested(QObject* client);
  void AlgorithmListRequested(QObject* client);

  void JobCreated(Job, QObject* client);
  void JobCreateError(QString, QObject*);
  void JobDeleted(Job, QObject* client);
  void JobDeleteError(QString path, QObject* client);

  void ImageCreated(Image, QObject* client);
  void ImageCreateError(QString path, QObject* client);
  void ImageDeleted(Image, QObject* client);
  void ImageDeleteError(QString path, QObject* client);
  void ImageCropped(Image, QObject* client);
  void ImageCropError(QString id, QObject* client);

  void ProfileCreated(Profile profile, QObject* client);
  void ProfileUpdated(Profile, QObject* client);
  void ProfileUpdateError(Profile::ID, QObject* client);
  void ProfileDeleted(Profile::ID profile, QObject* client);
  void ProfileDeleteError(Profile::ID profile, QObject* client);
  void DefaultSettingsProfileSet(Profile::ID profile, QObject* client);
  void DefaultSettingsProfileSetError(QString error, QObject* client);
  void SetStartingWell(Job::ID job, Profile::ID plate, int row, int col,
                       QObject* client);
  void SetStartingWellError(QString error, QObject* client);
  void SetColoniesToPick(Job::ID job, std::set<std::size_t> colonies,
                         QObject* client);
  void SetColoniesToPickError(QString error, QObject* client);

  void ColonyDetectionStarted(Job::ID, QObject* client);
  /**
   * @brief Always call this from ColonyDetector or derived!
   */
  void ColonyDetected(std::vector<Colony> const* detector, QObject* client);
  /**
   * @brief Always call this from ColonyDetector or derived!
   */
  void ColonyDetectionError(QString error, QObject* client);

  void Error(QString where, QString what, QObject* client);

 private:
  // TODO change name
  APIController* op;
};
}  // namespace c3picko
