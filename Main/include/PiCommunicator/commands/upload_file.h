#ifndef UPLOAD_COMMAND_H_
#define UPLOAD_COMMAND_H_

#include "PiCommunicator/command.h"
#include "PiCommunicator/datamodel/location.h"
#include "PiCommunicator/responses/upload_response.h"

class QFile;

namespace c3picko {
namespace pi {
namespace commands {
class UploadFile : public Command {
  Q_OBJECT

  UploadFile() = delete;
  using Command::Command;

 public:
  typedef responses::UploadResponse Response;

  /**
   * @brief Uploads a new file with content @p data to @p file_name in @p
   * location
   * [API](http://docs.octoprint.org/en/master/api/files.html#upload-file-or-create-folder)
   * @param content File content
   * @param location SdCard or local
   * @param file_name Name of file to create
   * @param select Should the file be selected?
   * @param print Should the file be printed?
   */
  static UploadFile* CreateFile(QByteArray content, data::Location location,
								QString file_name, bool select = false,
								bool print = false);

  /**
   * @brief Creates a new folder @p folder_path on location local.
   * NOTE SdCard else is not supported by octoprint yet
   * [API](http://docs.octoprint.org/en/master/api/files.html#upload-file-or-create-folder)
   * @param folder_name Name of the folder to create
   */
  static UploadFile* CreateFolder(QString folder_name, QString path,
								  data::Location location);

 protected:
  static QHttpMultiPart* BuildFileUploadPacket(QByteArray data,
											   QString file_name, bool select,
											   bool print);
  static QHttpMultiPart* BuildFolderCreatePacket(QString folder_name,
												 QString path);

 public slots:
  virtual void OnReplyFinished(QNetworkReply* reply) override;

 private:
};
}  // namespace commands
}  // namespace pi
}  // namespace c3picko

#endif	// UPLOAD_COMMAND_H_
