#ifndef UPLOAD_COMMAND_H_
#define UPLOAD_COMMAND_H_

#include "command.h"
#include "commands/responses/upload_response.h"

namespace c3picko {
namespace commands {
class UploadFile : public Command {
  Q_OBJECT

public:
  /// Uploads a new file with content #data to the location #file_path
  /// [API](http://docs.octoprint.org/en/master/api/files.html#upload-file-or-create-folder)
  /// \param path Must be either local or sdcard FIXME
  UploadFile(QByteArray data, QString path, QString file_name, bool select,
             bool print);
  /// Creates a new folder #folder_path
  /// [API](http://docs.octoprint.org/en/master/api/files.html#upload-file-or-create-folder)
  /// Local or SD card? FIXME
  UploadFile(QString folder_name, QString path);

protected:
  QByteArray BuildFileUploadPacket(QByteArray data, QString file_name,
                                   bool select, bool print);
  QByteArray BuildFolderCreatePacket(QString folder_name, QString path);

public slots:
  virtual void OnReplyFinished(QNetworkReply *reply) override;

private:
};
} // namespace commands
} // namespace c3picko

#endif // UPLOAD_COMMAND_H_
