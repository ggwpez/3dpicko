/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "PiCommunicator/commands/delete_file.h"

namespace d3picko {
namespace pi {
namespace commands {
DeleteFile* DeleteFile::Delete(data::Location location, QString path) {
  return new DeleteFile("files/" + data::ToString(location) + '/' + path, {204},
						HTTPType::DELETE);
}
}  // namespace commands
}  // namespace pi
}  // namespace d3picko
