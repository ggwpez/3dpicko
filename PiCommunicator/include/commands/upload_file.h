#ifndef UPLOAD_COMMAND_H_
#define UPLOAD_COMMAND_H_

#include "command.h"
#include "datamodel/location.h"
#include "responses/upload_response.h"

namespace c3picko
{
namespace pi
{
	namespace commands
	{
		class UploadFile : public Command
		{
			Q_OBJECT

		  public:
			/**
			 * @brief Uploads a new file with content @p data to @p file_name in @p location
			 * [API](http://docs.octoprint.org/en/master/api/files.html#upload-file-or-create-folder)
			 * @param content File content
			 * @param location SdCard or local
			 * @param file_name Name of file to create
			 * @param select Should the file be selected?
			 * @param print Should the file be printed?
			 */
			UploadFile(QByteArray content, data::Location location, QString file_name, bool select, bool print);

			/**
			 * @brief UploadFile Creates a new folder @p folder_path on location local.
			 * NOTE SdCard else is not supported by octoprint yet
			 * [API](http://docs.octoprint.org/en/master/api/files.html#upload-file-or-create-folder)
			 * @param folder_name Name of the folder to create
			 */
			UploadFile(QString folder_name, data::Location location);

			typedef responses::UploadResponse Response;

		  protected:
			QHttpMultiPart* BuildFileUploadPacket(QByteArray data, QString file_name, bool select, bool print);
			QHttpMultiPart* BuildFolderCreatePacket(QString folder_name);

		  public slots:
			virtual void OnReplyFinished(QNetworkReply* reply) override;

		  private:
		};
	} // namespace commands
}
} // namespace c3picko

#endif // UPLOAD_COMMAND_H_
