#include "include/requestmapper.h"
#include "include/global.h"
#include "include/apicontroller.h"
#include "httpsession.h"
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonDocument>

namespace c3picko {

RequestMapper::RequestMapper(StaticFileController* file_controller, APIController* api, QObject* parent)
	: HttpRequestHandler(parent), staticFileController(file_controller), api_(api) {
	// empty
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
	QByteArray path=request.getPath();
	qInfo("RequestMapper: path=%s",path.data());

	if (path.startsWith("/")) {
		staticFileController->service(request,response);
	}
	else {
		response.setStatus(404, "Not found");
		response.write("The URL is wrong, no such document.");
	}

	qInfo("RequestMapper: finished request");
}
} // namespace c3picko
