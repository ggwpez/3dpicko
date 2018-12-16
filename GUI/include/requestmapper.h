#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "include/apicontroller.h"
#include "httprequesthandler.h"
#include "staticfilecontroller.h"

using namespace stefanfrings;

class RequestMapper : public HttpRequestHandler {
	Q_OBJECT
public:
    RequestMapper(StaticFileController* file_controller, QObject* parent=nullptr);
	void service(HttpRequest& request, HttpResponse& response);
private:
	StaticFileController* staticFileController;
	APIController api;
};

#endif // REQUESTMAPPER_H
