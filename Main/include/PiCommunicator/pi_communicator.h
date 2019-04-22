#ifndef PICOMMUNICATOR_INCLUDE_PI_COMMUNICATOR_H_
#define PICOMMUNICATOR_INCLUDE_PI_COMMUNICATOR_H_

#include <QObject>
#include <QQueue>
#include <QString>
#include <QTimer>

#include "PiCommunicator/apikey.h"
#include "PiCommunicator/gcode.h"
#include "PiCommunicator/octoprint.h"
#include "PiCommunicator/print_job.h"

namespace c3picko {
namespace pi {
/**
 * @brief Connect and send GCode to an Octoprint printer.
 */
class PiCommunicator : public QObject {
  Q_OBJECT

 public:
  enum class State {
	CONNECTING,
	CONNECTED,
	DISCONNECTING,
	DISCONNECTED,  /// Default state, when PiCommunicator is created
	ERROR
  };

  PiCommunicator(const OctoConfig &config, QObject *_parent = nullptr);

 public slots:
  void Connect();
  void Disconnect();

  /**
   * @brief Starts the given job
   *
   * Status updates by @link OnStartedJob and @link OnStartingJobError
   * @return The created job, DONT DELETE IT
   */
  PrintJob const *StartJob(GCode gcode);
  /**
   * @brief TODO rethink this, not optimal
   */
  void CancelJob(PrintJob *);

 signals:
  void OnConnected();
  void OnConnectionError(QString error);
  void OnDisconnected();
  void OnDisconnectionError(QString error);

  void OnTransition(State from, State to);

 protected:
  /**
   * @brief Generates distinct filenames for naming the uploaded files
   * @return Filename
   */
  QString GenerateFilename() const;
  void Transition(State t);

 private slots:
  void Tick();
  void StartNextJob();
  void SendCommand(Command *cmd);

 private:
  /**
   * @brief OctoPrint only allows one active job at a time
   */
  PrintJob *current_job_;
  QQueue<PrintJob *> waiting_jobs_;

  State state_;
  OctoPrint const printer_;
  QTimer ticker_;
  const int tick_time_ms_ = 1000;
};
}  // namespace pi
}  // namespace c3picko

#endif  // PICOMMUNICATOR_INCLUDE_PI_COMMUNICATOR_H_
