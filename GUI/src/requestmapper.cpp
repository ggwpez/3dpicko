#include "requestmapper.h"
#include "httpsession.h"
#include "global.h"
#include <QFileInfo>
#include <QTime>
#include <QDir>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

HttpSessionStore* RequestMapper::sessionStore=nullptr;
 StaticFileController* RequestMapper::staticFileController=nullptr;

RequestMapper::RequestMapper(QObject* parent)
	: HttpRequestHandler(parent) {
	// empty
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
	QByteArray path=request.getPath();
	qDebug("RequestMapper: path=%s",path.data());

	/*
	QByteArray sessionId=sessionStore->getSessionId(request,response);
	if (sessionId.isEmpty() && path!="/login") {
		qDebug("RequestMapper: redirect to login page");
		response.redirect("/login");
		return;
	}
	*/

	if (path=="/" || path=="/hello") {
		helloWorldController.service(request, response);
	}
	else if (path=="/list") {
		listDataController.service(request, response);
	}
	else if (path=="/login") {
		loginController.service(request, response);
	}
	else if (path=="/cookie") {
		cookieTestController.service(request, response);
	}
	else if (path.startsWith("/files/select")) {
		response.write("Selection is now: " +request.getParameter("x") +"/" +request.getParameter("y"));
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
	else if (path.startsWith("/files") || path.startsWith(("/" +UploadFolderName()).toUtf8())) {
		staticFileController->service(request,response);
	}
	else {
		response.setStatus(404, "Not found");
		response.write("The URL is wrong, no such document.");
	}

	qDebug("RequestMapper: finished request");
}

