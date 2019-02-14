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
namespace c3picko
{
class APIController : public QObject
{
	Q_OBJECT
	friend class APIInput;
	friend class APIOutput;

  public:
	APIController(QThreadPool* pool, Database* db, QObject* parent);

	// Forwarded to APIInput
	void request(QJsonObject request, QObject* client);

  signals:
	void ToClient(QObject* client, QString type, QJsonObject data);
	void ToAll(QString type, QJsonObject data);
	void ToAllExClient(QObject* client, QString type, QJsonObject data);

  protected:
	Database&	db() const;
	QThreadPool* pool() const;

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

	/**
	 * @brief OnColonyDetected
	 * @param detector Needs to be deleted
	 * @param client
	 */
	void OnColonyDetected(ColonyDetector* detector, QObject* client);
	void OnColonyDetectionError(QString error, QObject* client);

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
	Database*	 db_;
	QThreadPool*  pool_;
	QSignalMapper mapper_;

	// Deleted by this
	APIOutput* output_;
	APIInput*  input_;
};
} // namespace c3picko
