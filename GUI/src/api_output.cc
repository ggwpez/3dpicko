#include "include/api_output.h"
#include "include/api_controller.h"

namespace c3picko {

	APIOutput::APIOutput(APIController* parent)
		: QObject((QObject*)parent), op(parent)
	{

	}

	void APIOutput::UnknownRequest(QString request, QObject* client)
	{

	}

	void APIOutput::ImageListRequested(QObject* client)
	{

	}

	void APIOutput::JobListRequested(QObject* client)
	{

	}

	void APIOutput::ProfileListRequested(QObject* client)
	{

	}

	void APIOutput::AlgorithmListRequested(QObject* client)
	{

	}

	void APIOutput::JobCreated(Job job, QObject* client)
	{

	}

	void APIOutput::JobCreateError(QString id, QObject* client)
	{

	}

	void APIOutput::JobDeleted(Job job, QObject* client)
	{

	}

	void APIOutput::JobDeleteError(QString path, QObject* client)
	{

	}

	void APIOutput::ImageCreated(Image image, QObject* client)
	{

	}

	void APIOutput::ImageCreateError(QString path, QObject* client)
	{

	}

	void APIOutput::ImageDeleted(Image image, QObject* client)
	{

	}

	void APIOutput::ImageDeleteError(QString path, QObject* client)
	{

	}

	void APIOutput::ImageCropped(Image image, QObject* client)
	{

	}

	void APIOutput::ImageCropError(QString id, QObject* client)
	{

	}

	void APIOutput::ProfileCreated(Profile::ID profile, QObject* client)
	{

	}

	void APIOutput::ProfileUpdated(Profile::ID profile, QObject* client)
	{

	}

	void APIOutput::ProfileUpdateError(Profile::ID profile, QObject* client)
	{

	}

	void APIOutput::ProfileDeleted(Profile profile, QObject* client)
	{

	}

	void APIOutput::ProfileDeleteError(Profile::ID profile, QObject* client)
	{

	}

	void APIOutput::ColonyDetected()
	{

	}

	void APIOutput::ColonyDetectionError(QString error, QObject* client)
	{

	}
}
