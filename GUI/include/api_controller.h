#pragma once

#include "httprequesthandler.h"
#include "include/global.h"
#include "include/types/job.hpp"
#include "database.hpp"
#include <QList>
#include <QString>
#include <QPointer>

class QThreadPool;
namespace c3picko {
	class APIInput;
	class APIOutput;
	class APIController : public QObject
	{
		Q_OBJECT
		friend class APIInput;
		friend class APIOutput;
	public:
		APIController(QThreadPool* pool, Database& db, QObject* parent);

		// Forwarded to APIInput
		void service(QJsonObject& request, QObject* client);

	protected:
		Database& db() const;
		QThreadPool* pool() const;

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

		void OnProfileCreated(Profile::ID profile, QObject* client);
		// not used
		// void OnProfileCreateError(Profile::ID profile, QObject* client);
		void OnProfileUpdated(Profile::ID, QObject* client);
		void OnProfileUpdateError(Profile::ID, QObject* client);
		void OnProfileDeleted(Profile profile, QObject* client);
		void OnProfileDeleteError(Profile::ID profile, QObject* client);

		void OnColonyDetected();
		void OnColonyDetectionError(QString error, QObject* client);

	protected:
		Database& db_;
		QThreadPool* pool_;

		// Deleted by this
		APIOutput* output_;
		APIInput* input_;
	};
}
