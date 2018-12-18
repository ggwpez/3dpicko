#ifndef APICONTROLLER_HPP
#define APICONTROLLER_HPP

#include "httprequesthandler.h"
#include "include/global.h"
#include <QList>
#include <QString>

using namespace stefanfrings;

namespace c3picko {

class APIController: public HttpRequestHandler {
	Q_OBJECT
public:
    APIController(QObject* parent=nullptr);
	void service(HttpRequest& request, HttpResponse& response);
};
} // namespace c3picko
#endif // APICONTROLLER_HPP
