#ifndef COOKIETESTCONTROLLER_H
#define COOKIETESTCONTROLLER_H

#include "httprequesthandler.h"

using namespace stefanfrings;

class CookieTestController: public HttpRequestHandler {
    Q_OBJECT
public:
    CookieTestController(QObject* parent=0);
    void service(HttpRequest& request, HttpResponse& response);
private:
    QList<QString> list;
};

#endif // COOKIETESTCONTROLLER_H
