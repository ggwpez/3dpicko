#ifndef APICONTROLLER_HPP
#define APICONTROLLER_HPP

#include "database.hpp"
#include "httprequesthandler.h"
#include "include/global.h"
#include "include/types/job.hpp"
#include <QList>
#include <QString>

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
  APIInput(APIController *parent);

  // WS Version
  void serviceRequest(QJsonObject &request, const QString &raw_request,
                      QObject *client);

signals:
  /**
   * @brief SendToClient Should send data to the client
   * @param data
   * @param client
   */
  void SendToClient(QJsonObject data, QObject *client);
  /**
   * @brief BroadcastToClients Should be connected to a SLOT that sends data to
   * all clients except the except client
   * @param data
   * @param except Client to be excluded
   */
  void SendToClients(QJsonObject data, QObject *except);

private:
  // TODO change name
  APIController *api;
};
} // namespace c3picko
#endif // APICONTROLLER_HPP
