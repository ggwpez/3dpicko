#include "include/api_controller.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QMetaMethod>

namespace c3picko
{

APIController::APIController(QThreadPool* pool, Database* db, QObject* parent)
	: QObject(parent), pool_(pool), db_(db), input_(new APIInput(this)), output_(new APIOutput(this))
{
	QMetaObject const* meta			   = this->metaObject();
	QMetaMethod		   default_handler = meta->method(meta->indexOfSlot(qPrintable("defaultSignalHandler()")));

	// TODO use QMetaObject::connectSlotsByName ?
	for (int i = meta->methodOffset(); i < meta->methodCount(); ++i)
	{
		QMetaMethod signal	= meta->method(i);
		QString		slot_name = QMetaObject::normalizedSignature(qPrintable(signal.methodSignature()));

		if (signal.methodType() == QMetaMethod::Signal)
		{
			if (!slot_name.startsWith("On"))
				continue;
			slot_name = slot_name.mid(2);
			QObject::connect(this, signal, this, default_handler);

			int slot_index = output_->metaObject()->indexOfSlot(qPrintable(slot_name));
			if (slot_index == -1)
				qDebug("Could not find slot APIOutput::%s", qPrintable(slot_name));
			{
				QMetaMethod slot = output_->metaObject()->method(slot_index);

				QObject::connect(this, signal, output_, slot);
			}
		}
	}
}

void APIController::defaultSignalHandler()
{
	QMetaObject const* meta_sender = QObject::sender()->metaObject();
	QString			   signal_name = QMetaObject::normalizedSignature(meta_sender->method(QObject::senderSignalIndex()).name());

	qDebug("Event %s::%s", qPrintable(meta_sender->className()), qPrintable(signal_name));
}

QJsonObject APIController::createImageList() const
{
	QJsonArray image_list;
	for (auto const& image : db_->images())
	{
		QJsonObject json;

		image.write(json);
		image_list.push_back(json);
	}

	return {{"images", image_list}};
}

QJsonObject APIController::createImageList(Image img)
{
	QJsonArray  json_jobs;
	QJsonObject json_job;

	img.write(json_job);
	json_jobs.append(json_job);

	return {{"images", json_jobs}};
}

QJsonObject APIController::createJobList()
{
	QJsonArray json_jobs;

	for (auto const& job : db_->jobs())
	{
		QJsonObject json;
		job.write(json);
		json_jobs.append(json);
	}

	return {{"jobs", json_jobs}};
}

QJsonObject APIController::createJobList(Job job)
{
	QJsonArray  json_jobs;
	QJsonObject json_job;

	job.write(json_job);
	json_jobs.append(json_job);

	return {{"jobs", json_jobs}};
}

QJsonObject APIController::createProfileList()
{
	QJsonArray json_jobs;

	for (auto const& profile : db_->profiles())
	{
		QJsonObject json;
		profile.write(json);
		json_jobs.append(json);
	}

	return {{"profiles", json_jobs}};
}

QJsonObject APIController::CreateAlgorithmList()
{
	return QJsonDocument::fromJson("{ \"321\": { name: \"Fluro\", description: \"Good for detecting fluorescent colonies\", settings: { "
								   "\"1234\": { name: \"Threshold "
								   "1\", type: \"slider\", valueType: \"float\", min: 0, max: 10, step: 0.1, defaultValue: 1.2, "
								   "description: \"\" }, \"123\": { name: "
								   "\"Erode & Dilate\", type: \"checkbox\", defaultValue: true, description: \"\" }, \"12sad3\": { name: "
								   "\"Erode & Dilate 2\", type: "
								   "\"checkbox\", defaultValue: false, description: \"do not touch me!\" } } }, \"423\": { name: \"Fluro "
								   "2\", description: \"Good for "
								   "detecting more fluorescent colonies\", settings: { \"1234\": { name: \"Thres\", type: \"slider\", "
								   "valueType: \"float\", min: -10, "
								   "max: 10, step: 1, defaultValue: 1, description: \"Hi this is very descriptive\" }, \"123\": { name: "
								   "\"Zahl\", type: \"slider\", "
								   "valueType: \"float\", min: -10, max: 10, step: 1, defaultValue: 1, description: \"\" } } } }",
								   nullptr)
		.object();
}

QJsonObject APIController::createDeleteImage(Image img) { return {{"id", img.id()}}; }

QJsonObject APIController::createDeleteJob(Job job) { return {{"id", job.id()}}; }

void APIController::request(QJsonObject request, QObject* client) { input_->serviceRequest(request, client); }

Database& APIController::db() const { return *db_; }

QThreadPool* APIController::pool() const { return pool_; }
} // namespace c3picko
