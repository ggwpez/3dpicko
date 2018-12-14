#include "include/apicontroller.h"
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

APIController::APIController(QObject* parent)
	: HttpRequestHandler(parent)
{

}

void APIController::service(HttpRequest& request, HttpResponse& response)
{
	QString path = request.getPath();

	if (! path.startsWith("/api"))
	{
		// error handling
		return;
	}
	path = path.mid(4);

	if (path.startsWith("/files/select")) {
		response.write("Selection is now: " +request.getParameter("x") +"/" +request.getParameter("y"));
	}
	else if (path.startsWith("/echo"))
	{
		response.write("echo\n");
		response.write(request.getBody());
	}
	else if (path.startsWith("/imageUpload")) {
		/*
		 * drag and drop image upload handler
		 * sends json array: ["filename", "lastModified"]
		*/

		//set path for images to /docroot/files/images/
		QString path = UploadFolder();

		//get uploaded image
		QTemporaryFile* file=request.getUploadedFile("files[]");
		//write image to path using original filename
		QFile output(path+request.getParameter("files[]"));
		if (! output.open(QIODevice::WriteOnly)) throw std::runtime_error("Could not open file");
		QByteArray data(file->readAll());
		output.write(data);

		//get file properties
		QFileInfo properties(output);
		//birthTime()?
		QJsonDocument json({properties.fileName(), properties.lastModified().toString("dd.MM.yy - HH:mm")});

		//write json array to response: [filename, lastModified]
		response.setHeader("Content-Type", "application/json; charset=utf-8");
		response.write(json.toJson(),true);

		/*
		QFileInfo properties(output);
		response.setHeader("Content-Type", "text/html; charset=utf-8");
		response.write(properties.birthTime().toString("HH:mm:ss").toUtf8());
		response.write(properties.fileName().toUtf8(), true);

		response.setHeader("Content-Type", "image/jpeg");
		response.write(data);
		*/
	}
	else if (path.startsWith("/getImageList")) {
		/*
		 * sends list of images with attributes, sorted by time
		 * json object: {"id":["filename","lastModified"], ...}
		*/
		QString path = UploadFolder();

		QDir imagesDir(path);
		QJsonObject fileInfo;
		int id = 0;
		for(QFileInfo &file : imagesDir.entryInfoList(QDir::Files, QDir::Time|QDir::Reversed)){
			//insert array ["filename","lastModified"] at key "id"
			fileInfo.insert(QString::number(id++), QJsonValue({ '/' +UploadFolderName() +'/' +file.fileName(), file.lastModified().toString("dd.MM.yy - HH:mm")}));
		}
		response.setHeader("Content-Type", "application/json; charset=utf-8");
		QJsonDocument data(fileInfo);
		response.write(data.toJson(),true);
	}
}
