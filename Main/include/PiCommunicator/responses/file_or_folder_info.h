/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QStringList>
#include <QVector>

#include "PiCommunicator/datamodel/file_type.h"
#include "PiCommunicator/datamodel/gcode_analysis.h"
#include "PiCommunicator/datamodel/location.h"
#include "PiCommunicator/datamodel/reference.h"
#include "PiCommunicator/response.h"

namespace c3picko {
namespace pi {
namespace responses {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#sec-api-datamodel-files-file)
 * TODO there may be additional properties see link above
 */
struct FileOrFolderInfo : public Response {
  FileOrFolderInfo() = default;
  FileOrFolderInfo(QJsonObject const& obj);

  QString name, display, path;
  data::FileType type;
  QStringList type_path;

  // File specific
  QString hash;
  qint64 date;
  data::Location origin;
  data::Reference refs;
  data::GCodeAnalysis gcode_analysis;

  // Folder specific
  QVector<FileOrFolderInfo> children;

  // Shared
  qint64 size;

 private:
  void ReadFileMember(QJsonObject const& obj);
  void ReadFolderMember(QJsonObject const& obj);
};
}  // namespace responses
}  // namespace pi
}  // namespace c3picko
