/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "GUI/api_controller.h"
#include "httprequesthandler.h"
#include "staticfilecontroller.h"

using namespace stefanfrings;

namespace d3picko {

/**
 * @brief Forwards requests to the StaticFileController
 */
class RequestMapper : public HttpRequestHandler {
  Q_OBJECT
 public:
  RequestMapper(StaticFileController* file_controller,
				QObject* parent = nullptr);
  void service(HttpRequest& request, HttpResponse& response);

 private:
  StaticFileController* staticFileController;
};
}  // namespace d3picko
#endif	// REQUESTMAPPER_H
