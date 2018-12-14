#include "include/commands/upload_file.h"
#include <QByteArray>
#include <QJsonValue>

#include <QHttpMultiPart>
#include <QHttpPart>

namespace c3picko
{
namespace pi
{
	namespace commands
	{
		// static QString const boundry1 = "----WebKitFormBoundaryDeC2E3iWbTv1PwMC";
		// static QString const boundry2 = "----WebKitFormBoundaryDeC2E3iWbTv1PwMC";

		UploadFile::UploadFile(QByteArray content, data::Location location, QString file_name, bool select, bool print)
			: Command("files/" + data::ToString(location), BuildFileUploadPacket(content, file_name, select, print), {201},
					  Command::HTTPType::POST, "multipart/form-data;")
		{
			content_type_ += ("; boundary=" + QString(query_->boundary()));
		}

		UploadFile::UploadFile(QString folder_name, QString path, data::Location location)
			: Command("files/" + data::ToString(location), BuildFolderCreatePacket(folder_name, path), {201}, Command::HTTPType::POST,
					  "multipart/form-data")
		{
			content_type_ += ("; boundary=" + QString(query_->boundary()));
		}

		QHttpMultiPart* UploadFile::BuildFileUploadPacket(QByteArray data, QString file_name, bool select, bool print)
		{
			/*QByteArray query;

			query.append("--" + boundry1 + "\nContent-Disposition: form-data; name=\"file\"; filename=\"" + file_name + "\"\n"
						 + "Content-Type: application/octet-stream\n\n");

			query.append(data);
			query.append("\n--" + boundry1 + "\nContent-Disposition: form-data; name=\"select\"" + "\n\n" + (select ? "true" : "false")
						 + '\n');
			query.append("--" + boundry1 + "\nContent-Disposition: form-data; name=\"print\"" + "\n\n" + (print ? "true" : "false")
						 + '\n');
			query.append("--" + boundry1 + "--\n");

			return query;*/

			// TODO this does sadly not work, since QHttpPart is not serializable
			// maybe it is in the future?
			QHttpMultiPart* query = new QHttpMultiPart(QHttpMultiPart::FormDataType);
			QHttpPart		file_part, select_part, print_part;

			{
				file_part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"file\"; filename=\"" + file_name + '"');
				file_part.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
				file_part.setBody(data);

				select_part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"select\"");
				select_part.setBody(select ? "true" : "false");

				print_part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"print\"");
				print_part.setBody(print ? "true" : "false");
			}

			query->append(file_part);
			query->append(select_part);
			query->append(print_part);

			return query;
		}

		QHttpMultiPart* UploadFile::BuildFolderCreatePacket(QString folder_name, QString path)
		{
			QHttpMultiPart* query = new QHttpMultiPart(QHttpMultiPart::FormDataType);
			QHttpPart		file_part, select_part;

			{
				file_part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"foldername\"");
				file_part.setBody(folder_name.toUtf8());

				if (path.length())
				{
					select_part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"path\"");
					select_part.setBody(path.toUtf8());
				}
			}

			query->append(file_part);
			query->append(select_part);

			return query;

			/*QString query;

			query.append("--" + boundry2 + "\nContent-Disposition: form-data; name=\"foldername\"\n\n");
			query.append(folder_name);

			query.append("\n--" + boundry2 + "\nContent-Disposition: form-data; name=\"path\"" + "\n\n");
			query.append("local");
			query.append("\n--" + boundry2 + "--\n");

			return query.toUtf8();*/
		}

		void UploadFile::OnReplyFinished(QNetworkReply* reply) { CheckStatusCodeAndResponse<Response>(reply); }
	} // namespace commands
}
} // namespace c3picko
