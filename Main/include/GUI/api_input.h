/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QList>
#include <QString>

#include "GUI/types/job.h"
#include "Main/global.h"
#include "database.h"
#include "httprequesthandler.h"

using namespace stefanfrings;

namespace c3picko {
class APIController;
/**
 * @brief Class for accepting user input and calling the proper response
 * functions from APIOutput
 */
class APIInput : public QObject {
  Q_OBJECT
 public:
  APIInput(APIController* parent);

  // WS Version
  void serviceRequest(QJsonObject& request, const QString& raw_request,
					  QObject* client);

 signals:
  /**
   * @brief SendToClient Should send data to the client
   * @param data
   * @param client
   */
  void SendToClient(QJsonObject data, QObject* client);
  /**
   * @brief BroadcastToClients Should be connected to a SLOT that sends data to
   * all clients except the except client
   * @param data
   * @param except Client to be excluded
   */
  void SendToClients(QJsonObject data, QObject* except);

 private:
  // TODO change name
  APIController* api;
};
}  // namespace c3picko
