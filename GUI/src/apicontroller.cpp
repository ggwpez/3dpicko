#include "include/apicontroller.h"
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>

namespace c3picko {

APIController::APIController(QObject* parent)
	: HttpRequestHandler(parent)
{

}

void APIController::service(QJsonObject& request, QJsonObject& response)
{
	QString path = request["request"].toString();
	QJsonObject req_data = request["data"].toObject();

	if (path.startsWith("getimagelist"))
	{
		/*
		 * sends list of images with attributes, sorted by time
		 * has to be an array to be sorted
		 * json array of objects:  [{"id" : "..." , "path" : "...", "filename" : "...", "upload_date" : "..."}, ...]
		*/
		QDir imagesDir(UploadFolder());
		QJsonArray image_list;
		for(QFileInfo &file : imagesDir.entryInfoList(QDir::Files, QDir::Time)){
			QJsonObject image_info{
				{"id", file.baseName().left(6)},
				{"path", '/' + UploadFolderName() + '/' + file.fileName()},
				{"filename", file.baseName().mid(6)},
				{"user_id", "default"},
				{"upload_date", file.lastModified().toString("dd.MM.yy - HH:mm")}
			};
			image_list.append(QJsonValue(image_info));
		}

		response = QJsonObject{
			{"images", image_list}
		};
	}
	else if (path.startsWith("deleteimage"))
	{
		QString filename = req_data["path"].toString().mid(1);
		qDebug() << "Deleting" << filename << "\nobj:" << request;
		QDir file(DocRoot());

		if(file.remove(filename)) response = QJsonObject{{ "deleted", QJsonArray{ req_data["id"].toString() } }};
		else response = QJsonObject{{ "deleted", {} }};
	}
	else if (path.startsWith("uploadimage"))
	{
		qDebug() << "Saving file";
		/*
		 * drag and drop image upload handler
		 * sends json object: {"id" : "id", "path" : "path_to_image", "filename" : "original_filename_without_extension", "upload_date" : "last_modified_date_string"}
		*/
		QString path = UploadFolder();

		//get uploaded image
		//write image to path using original filename with unique six-digit prefix
		QTemporaryFile output(path+"XXXXXX"+req_data["filename"].toString());
		//keep tmp file permanently
		output.setAutoRemove(false);
		if (! output.open()) throw std::runtime_error("Could not open file");
		QByteArray data(QByteArray::fromBase64(req_data["file"].toString().toUtf8()));
		output.write(data);

		//get file properties
		QFileInfo properties(output);
		response = QJsonObject
		{
			{"id", properties.baseName().left(6)},
			{"path", '/' + UploadFolderName() + '/' + properties.fileName()},
			{"filename", properties.baseName().mid(6)},
			{"user_id", "default"},
			{"upload_date", properties.lastModified().toString("dd.MM.yy - HH:mm")}
		};

		OnNewFile(output.fileName());
	}
    else if (path.startsWith("createsettingsprofile")){
        QJsonObject json_profile_object = request["data"].toObject();
        // TODO create new profile
        // TODO create unique id
        QString newId = QString::number(QRandomGenerator::global()->generate());
        json_profile_object["id"] = newId;
        response = json_profile_object;
    }
    else if (path.startsWith("updatesettingsprofile")){
        QJsonObject json_profile_object = request["data"].toObject();
        // TODO update profile
        response = json_profile_object;
    }
	else if (path.startsWith("createjob"))
	{
		response = QJsonObject({{"id", 123}});
	}
	else
	{
		response = QJsonObject({{"error", "unknown request"}});
	}
}
} // namespace c3picko
