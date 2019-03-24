#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "httprequesthandler.h"
#include "include/api_controller.h"
#include "staticfilecontroller.h"

using namespace stefanfrings;

namespace c3picko {

class RequestMapper : public HttpRequestHandler {
  Q_OBJECT
 public:
  RequestMapper(StaticFileController *file_controller,
                QObject *parent = nullptr);
  void service(HttpRequest &request, HttpResponse &response);

 private:
  StaticFileController *staticFileController;
};
}  // namespace c3picko
#endif  // REQUESTMAPPER_H
