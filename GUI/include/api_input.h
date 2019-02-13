#ifndef APICONTROLLER_HPP
#define APICONTROLLER_HPP

#include "httprequesthandler.h"
#include "include/global.h"
#include "include/types/job.hpp"
#include "database.hpp"
#include <QList>
#include <QString>

using namespace stefanfrings;

namespace c3picko {
class APIController;
/**
 * @brief Class for accepting user input and calling the proper response functions from APIOutput
 */
class APIInput : public QObject {
	Q_OBJECT
public:
	APIInput(APIController* parent);

	// WS Version
	void service(QJsonObject& request, QObject* client);

	QJsonObject createImageList();
	QJsonObject createImageList(Image);
	QJsonObject createJobList();
	QJsonObject createJobList(Job);
	QJsonObject createDeleteImage(Image);
	QJsonObject createDeleteJob(Job job);
	QJsonObject createProfileList();

signals:
	/**
	 * @brief SendToClient Should send data to the client
	 * @param data
	 * @param client
	 */
	void SendToClient(QJsonObject data, QObject* client);
	/**
	 * @brief BroadcastToClients Should be connected to a SLOT that sends data to all clients except the except client
	 * @param data
	 * @param except Client to be excluded
	 */
	void SendToClients(QJsonObject data, QObject* except);

private:
	// TODO change name
	APIController* api;
};
} // namespace c3picko
#endif // APICONTROLLER_HPP
