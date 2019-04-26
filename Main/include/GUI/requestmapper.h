#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "GUI/api_controller.h"
#include "httprequesthandler.h"
#include "staticfilecontroller.h"

using namespace stefanfrings;

namespace c3picko {

/**
 * @brief Forwards requests to the StaticFileController
 */
class RequestMapper : public HttpRequestHandler {
  Q_OBJECT
 public:
  RequestMapper(StaticFileController* file_controller,
				QObject* parent = nullptr);
  void service(HttpRequest& request, HttpResponse& response);

 private:
  StaticFileController* staticFileController;
};
}  // namespace c3picko
#endif  // REQUESTMAPPER_H