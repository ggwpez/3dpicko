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
	void OnNewFile(Image, QObject* client);
	void OnNewJob(Job, QObject* client);
	void OnFileDeleted(Image, QObject* client);
	void OnJobDeleted(Job, QObject* client);
	void OnFileUploadError(QString path, QObject* client);
	void OnJobCreateError(QString, QObject*);
	void OnFileDeleteError(QString path, QObject* client);
	void OnJobDeleteError(QString path, QObject* client);
	void OnFileCropped(Image, QObject* client);
	void OnFileCropError(QString id, QObject* client);

private:
	Database& db_;
};
} // namespace c3picko
#endif // APICONTROLLER_HPP
