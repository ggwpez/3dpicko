#ifndef RETRIVE_RESPONSE_H_
#define RETRIVE_RESPONSE_H_

#include "commands/responses/file_information.h"
#include "commands/responses/response.h"

#include <QVector>

namespace c3picko {
namespace commands {
namespace responses {
/**
 * @brief @link
 * http://docs.octoprint.org/en/master/api/files.html#sec-api-fileops-datamodel-retrieveresponse
 * @endlink
 */
struct RetriveResponse : Response {
  RetriveResponse(QJsonObject);

  QVector<FileInformation> files;
  QString free;
};
} // namespace responses
} // namespace commands
} // namespace c3picko

#endif // RETRIVE_RESPONSE_H_
