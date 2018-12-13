#ifndef PICOMMUNICATOR_INCLUDE_PI_COMMUNICATOR_H_
#define PICOMMUNICATOR_INCLUDE_PI_COMMUNICATOR_H_

#include <QObject>
#include <QString>
#include <QVector>

#include "include/apikey.h"
#include "include/gcode.h"
#include "include/octoprint.h"
#include "include/print_job.h"

namespace c3picko
{
namespace pi
{
	/**
	 * @brief Connect and send GCode to an Octoprint printer
	 */
	class PiCommunicator : public QObject
	{
		Q_OBJECT

	  public:
		enum class State
		{
			CONNECTING,
			CONNECTED,
			DISCONNECTING,
			DISCONNECTED, /// Default state, when PiCommunicator is created
			ERROR
		};

		PiCommunicator(QString ip, ApiKey key, QObject* _parent = nullptr);

	  public slots:
		void Connect();
		void Disconnect();

		/**
		 * @brief Starts the given job
		 *
		 * Status updates by @link OnStartedJob and @link OnStartingJobError
		 * @return The created job, must be deleted my caller.
		 */
		PrintJob* StartJob(GCode gcode);

	  signals:
		void OnConnected();
		void OnConnectionError(QString error);
		void OnDisconnected();
		void OnDisconnectionError(QString error);

	  protected:
		/**
		 * @brief Generates distinct filenames for naming the uploaded files
		 * @return Filename
		 */
		QString gen_filename() const;

	  private:
		OctoPrint printer_;
	};
}
} // namespace c3picko

#endif // PICOMMUNICATOR_INCLUDE_PI_COMMUNICATOR_H_
