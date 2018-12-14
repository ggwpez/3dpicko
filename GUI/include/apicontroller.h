#ifndef APICONTROLLER_HPP
#define APICONTROLLER_HPP

#include "httprequesthandler.h"
#include "include/global.h"
#include <QList>
#include <QString>

using namespace stefanfrings;

class APIController: public HttpRequestHandler {
	Q_OBJECT
public:
	APIController(QObject* parent=0);
	void service(HttpRequest& request, HttpResponse& response);
};

#endif // APICONTROLLER_HPP
