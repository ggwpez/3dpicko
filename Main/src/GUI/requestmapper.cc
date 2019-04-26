#include "GUI/requestmapper.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include "GUI/api_input.h"
#include "Main/global.h"
#include "httpsession.h"

namespace c3picko {

RequestMapper::RequestMapper(StaticFileController* file_controller,
							 QObject* parent)
	: HttpRequestHandler(parent), staticFileController(file_controller) {
  // empty
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
  QByteArray path = request.getPath();
  qInfo("RequestMapper: path=%s", path.data());

  if (path.startsWith("/")) {
	staticFileController->service(request, response);
  } else {
	response.setStatus(404, "Not found");
	response.write("The URL is wrong, no such document.");
  }

  qInfo("RequestMapper: finished request");
}
}  // namespace c3picko