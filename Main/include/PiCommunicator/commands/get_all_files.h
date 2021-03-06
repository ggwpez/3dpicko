/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef GET_ALL_FILES_H_
#define GET_ALL_FILES_H_

#include "PiCommunicator/command.h"
#include "PiCommunicator/responses/retrive_response.h"

namespace d3picko {
namespace pi {
namespace commands {
/**
 * @brief [API](http://docs.octoprint.org/en/master/api/files.html#id2)
 */
class GetAllFiles : public Command {
  Q_OBJECT

  GetAllFiles() = delete;
  using Command::Command;

 public:
  typedef responses::RetriveResponse Response;

  static GetAllFiles* AllFiles();
  static GetAllFiles* AllFilesRecursive();

 public slots:
  virtual void OnReplyFinished(QNetworkReply* reply) override;
};
}  // namespace commands
}  // namespace pi
}  // namespace d3picko

#endif	// GET_ALL_FILES_H_
