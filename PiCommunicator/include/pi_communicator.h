#ifndef PICOMMUNICATOR_INCLUDE_PI_COMMUNICATOR_H_
#define PICOMMUNICATOR_INCLUDE_PI_COMMUNICATOR_H_

#include <QObject>
#include <QString>
#include <QVector>

#include "include/apikey.h"
#include "include/octoprint.h"

namespace c3picko
{
typedef QVector<QString> GCode;
typedef int				 JobId;

/**
 * @brief Connect and send GCode to an Octoprint printer
 */
class PiCommunicator : public QObject
{
	Q_OBJECT

  public:
	PiCommunicator(QString ip, ApiKey key, QObject* _parent = nullptr);

  public slots:
	void Connect();
	void Disconnect();

	/**
	 * @brief Starts the given job
	 *
	 * Status updates by @link OnStartedJob and @link OnStartingJobError
	 */
	void StartJob(GCode);

  signals:
	void OnConnected();
	void OnConnectionError(QString error);
	void OnDisconnected();
	void OnDisconnectionError(QString error);

	void OnStartedJob(JobId);
	void OnStartingJobError(JobId, QString error);

  private:
	OctoPrint printer_;
};
} // namespace c3picko

#endif // PICOMMUNICATOR_INCLUDE_PI_COMMUNICATOR_H_
