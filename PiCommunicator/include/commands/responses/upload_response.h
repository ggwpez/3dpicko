#ifndef UPLOAD_RESPONSE_H_
#define UPLOAD_RESPONSE_H_

#include "commands/responses/abridged_file_info.h"
#include "commands/responses/datamodel/origin.h"
#include "commands/responses/response.h"

#include <QVector>

namespace c3picko {
namespace commands {
namespace responses {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/files.html#sec-api-fileops-datamodel-retrieveresponse)
 */
struct UploadResponse : Response {
  UploadResponse(QJsonObject obj);

  AbridgedFileInfo local, sdcard, folder;
  bool done;

  /// Not in the documentation but needed to remember the type
  Origin origin;
  /// Is it a folder? Otherwise its a file
  bool is_folder;
};
} // namespace responses
} // namespace commands
} // namespace c3picko

#endif // RETRIVE_RESPONSE_H_
