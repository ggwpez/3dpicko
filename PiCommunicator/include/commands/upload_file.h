#ifndef UPLOAD_COMMAND_H_
#define UPLOAD_COMMAND_H_

#include "command.h"
#include "commands/responses/upload_response.h"

namespace c3picko
{
namespace commands
{
	class UploadFile : public Command
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Uploads a new file with content @p data to the location @p file_path
		 * [API](http://docs.octoprint.org/en/master/api/files.html#upload-file-or-create-folder)
		 * @param data File content
		 * @param path Folder path of file
		 * @param file_name Name of file to create
		 * @param select Should the file be selected?
		 * @param print Should the file be printed?
		 */
		UploadFile(QByteArray data, QString path, QString file_name, bool select, bool print);

		/**
		 * @brief UploadFile Creates a new folder @p folder_path
		 * [API](http://docs.octoprint.org/en/master/api/files.html#upload-file-or-create-folder)
		 * @param folder_name Name of the folder to create
		 * @param path Path to the folder location
		 *
		 * Local of SD card? FIXME
		 */
		UploadFile(QString folder_name, QString path);

		typedef responses::UploadResponse Response;

	  protected:
		QByteArray BuildFileUploadPacket(QByteArray data, QString file_name, bool select, bool print);
		QByteArray BuildFolderCreatePacket(QString folder_name, QString path);

	  public slots:
		virtual void OnReplyFinished(QNetworkReply* reply) override;

	  private:
	};
} // namespace commands
} // namespace c3picko

#endif // UPLOAD_COMMAND_H_
