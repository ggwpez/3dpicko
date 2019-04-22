#ifndef PRINT_JOB_H
#define PRINT_JOB_H

#include <QObject>
#include "PiCommunicator/datamodel/progress_info.h"
#include "PiCommunicator/gcode.h"

namespace c3picko {
namespace pi {
class PrintJob : public QObject {
  Q_OBJECT

 public:
  enum class State {
	// Start state
	NEW,

	UPLOADED,
	RUNNING,
	PAUSED,

	// Accepting states
	CANCELED,
	DONE
  };
  typedef qreal ProgressType;

  PrintJob(GCode gcode);

 public slots:
  void UpdateProgress(data::ProgressInfo new_info);

 signals:
  void OnError(QString);
  /**
   * @brief Job was successfully send to the printer,
   * either as UploadFile or ArbitratyCommand
   */
  void OnUploaded();
  /**
   * @brief Job started printing
   */
  void OnStarted();
  void OnCanceled();
  void OnRestarted();
  void OnPaused();
  void OnResumed();
  void OnToggled();
  void OnProgressUpdate(data::ProgressInfo);

 public:
  State state;
  GCode const gcode;
  data::ProgressInfo progress;
};
}  // namespace pi
}  // namespace c3picko

#endif  // PRINT_JOB_H
