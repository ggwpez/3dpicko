#include "include/apicontroller.h"
#include "include/imagehandler.h"
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

namespace c3picko {

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

    if (path.startsWith("/echo"))
	{
		response.write("echo\n");
		response.write(request.getBody());
	}
    else if (path.startsWith("/uploadimage")) {
		/*
		 * drag and drop image upload handler
         * sends json object: {"id" : "id", "path" : "path_to_image", "filename" : "original_filename_without_extension", "upload_date" : "last_modified_date_string"}
		*/
        QString path = UploadFolder();

		//get uploaded image
        QTemporaryFile* file = request.getUploadedFile("files[]");
        //write image to path using original filename with unique six-digit prefix
        QTemporaryFile output(path+"XXXXXX"+request.getParameter("files[]"));
        //keep tmp file permanently
        output.setAutoRemove(false);
        if (! output.open()) throw std::runtime_error("Could not open file");
		QByteArray data(file->readAll());
		output.write(data);

        //get file properties
		QFileInfo properties(output);
        QJsonObject image_info{
            {"id", properties.baseName().left(6)},
            {"path", '/' + UploadFolderName() + '/' + properties.fileName()},
            {"filename", properties.baseName().mid(6)},
            {"user_id", "default"},
            {"upload_date", properties.lastModified().toString("dd.MM.yy - HH:mm")}
        };

        //write image_info object to response
		response.setHeader("Content-Type", "application/json; charset=utf-8");
        response.write(QJsonDocument(image_info).toJson(),true);
	}
    else if (path.startsWith("/getimagelist")) {
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
		response.setHeader("Content-Type", "application/json; charset=utf-8");
        response.write(QJsonDocument(image_list).toJson(),true);
	}
    else if (path.startsWith("/deleteimage")){
        QString filename = request.getParameter("path").split('/').last();
        QDir file(UploadFolder());
        response.setHeader("Content-Type", "text/plain");
        if(file.remove(filename)) response.write("successfully deleted " + filename.toUtf8(), true);
        else response.write("deletion failed", true);
    }
}
} // namespace c3picko
