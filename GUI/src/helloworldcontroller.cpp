#include "helloworldcontroller.h"

HelloWorldController::HelloWorldController(QObject* parent)
    : HttpRequestHandler(parent) {
    // empty
}

void HelloWorldController::service(HttpRequest &request, HttpResponse &response) {
    response.write("Hello World",true);
}
