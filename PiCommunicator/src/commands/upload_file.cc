#include "include/commands/upload_file.h"
#include <QByteArray>
#include <QJsonValue>

#include <QHttpMultiPart>
#include <QHttpPart>

namespace c3picko
{
namespace commands
{
	static QString const boundry1 = "----WebKitFormBoundaryDeC2E3iWbTv1PwMC";
	static QString const boundry2 = "----WebKitFormBoundaryDeC2E3iWbTv1PwMD";

	UploadFile::UploadFile(QByteArray data, QString path, QString file_name, bool select, bool print)
		: Command("files/" + path, BuildFileUploadPacket(data, file_name, select, print), {201},
				  "multipart/form-data; boundary=" + boundry1)
	{
	}

	UploadFile::UploadFile(QString folder_name, QString path)
		: Command("files", BuildFolderCreatePacket(folder_name, path), {201}, "multipart/form-data; boundary=" + boundry2)
	{
	}

	QByteArray UploadFile::BuildFileUploadPacket(QByteArray data, QString file_name, bool select, bool print)
	{
		QByteArray query;

		query.append("--" + boundry1 + "\nContent-Disposition: form-data; name=\"file\"; filename=\"" + file_name + "\"\n"
					 + "Content-Type: application/octet-stream\n\n");

		query.append(data);
		query.append("\n--" + boundry1 + "\nContent-Disposition: form-data; name=\"select\"" + "\n\n" + (select ? "true" : "false")
					 + '\n');
		query.append("--" + boundry1 + "\nContent-Disposition: form-data; name=\"print\" " + "\n\n" + (print ? "true" : "false") + '\n');
		query.append("--" + boundry1 + "--");

		return query;

		// TODO this does sadly not work, since QHttpPart is not serializable
		// maybe it is in the future?
		/*QHttpMultiPart query(QHttpMultiPart::FormDataType);
		QHttpPart file_part, select_part, print_part;

		{
			file_part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"file\"; filename=" + file_path + '"');
			file_part.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
			file_part.setBody(data);

			select_part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"select\"");
			select_part.setBody(select ? "true"	: "false");

			print_part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"print\"");
			print_part.setBody(print ? "true" : "false");
		}

		query.append(file_part);
		query.append(select_part);
		query.append(print_part);

		return query.da*/
	}

	QByteArray UploadFile::BuildFolderCreatePacket(QString folder_name, QString path)
	{
		QString query;

		query.append("--" + boundry2 + "\nContent-Disposition: form-data; name=\"foldername\"\n\n");
		query.append(folder_name);

		query.append("\n--" + boundry2 + "\nContent-Disposition: form-data; name=\"path\"" + "\n\n");
		query.append(path);
		query.append("\n--" + boundry2 + "--");

		return query.toUtf8();
	}

	void UploadFile::OnReplyFinished(QNetworkReply* reply) { OnReplyFinishedDefault<Response>(reply); }
}
}
