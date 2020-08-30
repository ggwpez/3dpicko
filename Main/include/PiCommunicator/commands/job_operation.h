/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef JOB_OPERATION_H
#define JOB_OPERATION_H

#include "PiCommunicator/command.h"
//#include "datamodel/job_action.h"
//#include "datamodel/job_pause_action.h"
#include "PiCommunicator/responses/job_info.h"

namespace c3picko {
namespace pi {
namespace commands {
/**
 * @brief Job control for the currently running job on OctoPrint
 */
class JobOperation : public Command {
  Q_OBJECT

  JobOperation() = delete;
  using Command::Command;

 public:
  typedef responses::JobInfo Response;

  static JobOperation* GetInfo();

  static JobOperation* Start();
  static JobOperation* Cancel();
  static JobOperation* Restart();
  static JobOperation* Pause();
  static JobOperation* Resume();
  static JobOperation* TogglePause();

 public slots:
  virtual void OnReplyFinished(QNetworkReply* reply) override;
};
}  // namespace commands
}  // namespace pi
}  // namespace c3picko

#endif	// JOB_OPERATION_H
