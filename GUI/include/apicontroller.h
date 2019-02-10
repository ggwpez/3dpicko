#ifndef APICONTROLLER_HPP
#define APICONTROLLER_HPP

#include "httprequesthandler.h"
#include "include/global.h"
#include "include/types/job.hpp"
#include "database.hpp"
#include <QList>
#include <QString>

using namespace stefanfrings;

namespace c3picko {

class APIController : public QObject {
	Q_OBJECT
public:
	APIController(Database& db, QObject* parent=nullptr);

	// WS Version
	void service(QJsonObject& request, QJsonObject& response, QObject* client);

	QJsonObject createImageList();
	QJsonObject createImageList(Image);
	QJsonObject createJobList();
	QJsonObject createJobList(Job);
	QJsonObject createDeleteImage(Image);
	QJsonObject createDeleteJob(Job job);
	QJsonObject createProfileList();

signals:
	/**
	 * @brief SendToClient Should send data to the client
	 * @param data
	 * @param client
	 */
	void SendToClient(QJsonObject data, QObject* client);
	/**
	 * @brief BroadcastToClients Should be connected to a SLOT that sends data to all clients except the except client
	 * @param data
	 * @param except Client to be excluded
	 */
	void SendToClients(QJsonObject data, QObject* except);

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

	void OnProfileCreated(Profile::ID profile, QObject* client);
	// not used
	// void OnProfileCreateError(Profile::ID profile, QObject* client);
	void OnProfileUpdated(Profile::ID, QObject* client);
	void OnProfileUpdateError(Profile::ID, QObject* client);
	void OnProfileDeleted(Profile profile, QObject* client);
	void OnProfileDeleteError(Profile profile, QObject* client);

	//void OnColonyDetected();
	void OnColonyDetectionError();

private:
	Database& db_;
};
} // namespace c3picko
#endif // APICONTROLLER_HPP
