#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "include/apicontroller.h"
#include "httprequesthandler.h"
#include "httpsessionstore.h"
#include "helloworldcontroller.h"
#include "listdatacontroller.h"
#include "logincontroller.h"
#include "cookietestcontroller.h"
#include "staticfilecontroller.h"

using namespace stefanfrings;

class RequestMapper : public HttpRequestHandler {
	Q_OBJECT
public:
	RequestMapper(StaticFileController* file_controller, QObject* parent=0);
	void service(HttpRequest& request, HttpResponse& response);
private:
	/*HelloWorldController helloWorldController;
	ListDataController listDataController;
	LoginController loginController;
	CookieTestController cookieTestController;*/
	StaticFileController* staticFileController;
	APIController api;
};

#endif // REQUESTMAPPER_H
