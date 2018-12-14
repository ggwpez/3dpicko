#include "cookietestcontroller.h"

CookieTestController::CookieTestController(QObject* parent)
    : HttpRequestHandler(parent) {
    // empty
}

void CookieTestController::service(HttpRequest &request, HttpResponse &response) {

    QByteArray cookie=request.getCookie("testcookie");
    if (!cookie.isEmpty()) {
        response.write("Found cookie with value: "+cookie,true);
    }
    else {
        HttpCookie cookie("testcookie","123456789",60);
        response.setCookie(cookie);
        response.write("A new cookie has been created.",true);
    }

}
