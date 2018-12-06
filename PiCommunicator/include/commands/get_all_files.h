#ifndef GET_ALL_FILES_H_
#define GET_ALL_FILES_H_

#include "command.h"
#include "responses/retrive_response.h"

namespace c3picko {
namespace commands {
/**
 * @brief [API](http://docs.octoprint.org/en/master/api/files.html#id2)
 * @endlink
 */
class GetAllFiles : public Command {
  Q_OBJECT

public:
  GetAllFiles(bool recursive);

  typedef responses::RetriveResponse Response;

public slots:
  virtual void OnReplyFinished(QNetworkReply *reply) override;
};
} // namespace commands
} // namespace c3picko

#endif // GET_ALL_FILES_H_
