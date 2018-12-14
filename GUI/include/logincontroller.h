#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H

#include "httprequesthandler.h"

using namespace stefanfrings;

class LoginController : public HttpRequestHandler {
    Q_OBJECT
public:
    LoginController(QObject* parent=0);
    void service(HttpRequest& request, HttpResponse& response);
};

#endif // LOGINCONTROLLER_H
