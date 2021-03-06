/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/pi_communicator.h"

#include <QString>

#include "PiCommunicator/commands/all.h"

namespace d3picko {
namespace pi {
PiCommunicator::PiCommunicator(OctoConfig const& config, QObject* _parent)
	: QObject(_parent),
	  current_job_(nullptr),
	  state_(State::DISCONNECTED),
	  printer_(config, this),
	  ticker_(this) {
  QObject::connect(&ticker_, SIGNAL(timeout()), this, SLOT(Tick()));
  ticker_.start(tick_time_ms_);
}

void PiCommunicator::Connect() {
  if (state_ != State::DISCONNECTED)
	return emit OnConnectionError(
		"Connect() can only be called in State::DISCONNECTED");

  /*Transition(State::CONNECTING);
  commands::Connection* cmd = commands::Connection::GetInfo();

  connect(cmd, &Command::OnStatusOk, this, [this, cmd](int status,
  commands::Connection::Response*) { Transition(State::CONNECTED); emit
  OnConnected();
  });
  connect(cmd, &Command::OnStatusErr, this / * ctx ,
																  [this](QJsonValue
  code) { emit OnConnectionError("Wrong status code: " + code.toString()); });
  connect(cmd, SIGNAL(OnNetworkErr()), this, SIGNAL(OnNetworkErr()));
  // Delete
  connect(cmd, SIGNAL(OnFinished()), cmd, SLOT(deleteLater()));*/
}

void PiCommunicator::Disconnect() {
  if (state_ != State::CONNECTED)
	return emit OnDisconnectionError(
		"Disconnect() can only be called in State::CONNECTED");

  Transition(State::DISCONNECTING);
}

void PiCommunicator::StartNextJob() {}

void PiCommunicator::SendCommand(Command* cmd) { (void)cmd; }

void PiCommunicator::Tick() {
  if (current_job_ && current_job_->state == PrintJob::State::RUNNING) {
	// commands::JobOperation* cmd = new commands::JobOperation;

	// connect(cmd, &Command::OnStatusErr, [this](QVariant, Command::Response*
	// response_base) { commands::JobOperation::Response* response =
	// static_cast<commands::JobOperation::Response*>(response_base);

	// current_job_-> FIXME here
	//	});
  }
}

const PrintJob* PiCommunicator::StartJob(GCode gcode) {
  PrintJob* job = new PrintJob(gcode);

  // We only have to put the job in the waiting queue, the tick function will
  // preempt it for execution
  waiting_jobs_.enqueue(job);

  return job;

  /*QByteArray data = gcode.join('\n').toUtf8();
  QString	name = GenerateFilename();

  commands::UploadFile* cmd = new commands::UploadFile(data,
  data::Location::LOCAL, name, true, true);

  QObject::connect(cmd, SIGNAL(OnStatusOk(int, Response*)), job,
  SIGNAL(OnUploaded())); QObject::connect(cmd, &Command::OnStatusErr, this,
  &PiCommands::OnStatusErr); QObject::connect(cmd,
  SIGNAL(OnNetworkErr(QString)), job, SIGNAL(OnError(QString)));
  QObject::connect(this, &PiCommands::done, &app, &QCoreApplication::quit);

  return job;*/
}

void PiCommunicator::CancelJob(PrintJob* job) {
  Q_ASSERT(job);

  if (job->state == PrintJob::State::DONE ||
	  job->state == PrintJob::State::CANCELED)
	return emit job->OnError(
		"Can only cancel jobs that are not DONE or CANCELED");

  if (job == current_job_) {
  } else if (waiting_jobs_.contains(job)) {
	waiting_jobs_.removeOne(job);
	job->state = PrintJob::State::CANCELED;
	emit job->OnCanceled();
  }
}

QString PiCommunicator::GenerateFilename() const {
  QByteArray data(16, 0);
  for (int i = 0; i < data.length(); ++i) data[i] = (qrand() % 26) + 'a';

  return "_3dpicko-" + QString(data.toHex()) + ".gcode";
}

void PiCommunicator::Transition(State t) {
  State old = state_;
  state_ = t;

  emit OnTransition(old, state_);
}
}  // namespace pi
}  // namespace d3picko
