#include <QTime>
#include "logincontroller.h"
#include "requestmapper.h"

LoginController::LoginController(QObject* parent)
    :HttpRequestHandler(parent) {
    // empty
}

void LoginController::service(HttpRequest &request, HttpResponse &response) {
    HttpSession session=RequestMapper::sessionStore->getSession(request,response,true);
    QByteArray username=request.getParameter("username");
    QByteArray password=request.getParameter("password");

    qDebug("username=%s",username.constData());
    qDebug("password=%s",password.constData());

    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
    response.write("<html><body>");

    if (session.contains("username")) {
        QByteArray username=session.get("username").toByteArray();
        QTime logintime=session.get("logintime").toTime();
        response.write("You are alread logged in.<br>");
        response.write("Your name is: "+username+"<br>");
        response.write("You logged in at: "+logintime.toString("HH:mm:ss").toLocal8Bit()+"<br>");
    }
    else {
        if (username=="test" and password=="hello") {
            response.write("Yes, correct");
            session.set("username",username);
            session.set("logintime",QTime::currentTime());
        }
        else {
            response.write("<form method='POST' action='/login'>");
            if (!username.isEmpty()) {
                response.write("No, that was wrong!<br><br>");
            }
            response.write("€ Please log in:<br>");
            response.write("Name:  <input type='text' name='username'><br>");
            response.write("Password: <input type='password' name='password'><br>");
            response.write("<input type='submit'>");
            response.write("</form");
        }
    }

    response.write("</body></html>",true);
}
