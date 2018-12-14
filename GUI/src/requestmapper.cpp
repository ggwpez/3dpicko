#include "include/requestmapper.h"
#include "include/global.h"
#include "httpsession.h"
#include <QFileInfo>
#include <QTime>
#include <QDir>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

RequestMapper::RequestMapper(StaticFileController* file_controller, QObject* parent)
	: HttpRequestHandler(parent), staticFileController(file_controller), api(this) {
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

	if (path.startsWith("/api"))
		api.service(request, response);
	else if (path.startsWith("/files") || path.startsWith(("/" +UploadFolderName()).toUtf8())) {
		staticFileController->service(request,response);
	}
	else {
		response.setStatus(404, "Not found");
		response.write("The URL is wrong, no such document.");
	}

	qDebug("RequestMapper: finished request");
}

