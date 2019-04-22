#include "PiCommunicator/commands/upload_file.h"
#include <QByteArray>
#include <QFile>
#include <QJsonValue>

#include <QHttpMultiPart>
#include <QHttpPart>

namespace c3picko {
namespace pi {
namespace commands {
UploadFile *UploadFile::CreateFile(QByteArray content, data::Location location,
								   QString file_name, bool select, bool print) {
  QHttpMultiPart *query =
	  BuildFileUploadPacket(content, file_name, select, print);

  return new UploadFile(
	  "files/" + data::ToString(location), query, {201},
	  Command::HTTPType::POST,
	  "multipart/form-data; boundary=" + QString(query->boundary()));
}

UploadFile *UploadFile::CreateFolder(QString folder_name, QString path,
									 data::Location location) {
  QHttpMultiPart *query = BuildFolderCreatePacket(folder_name, path);

  return new UploadFile(
	  "files/" + data::ToString(location), query, {201},
	  Command::HTTPType::POST,
	  "multipart/form-data; boundary=" + QString(query->boundary()));
}

QHttpMultiPart *UploadFile::BuildFileUploadPacket(QByteArray data,
												  QString file_name,
												  bool select, bool print) {
  QHttpMultiPart *query = new QHttpMultiPart(QHttpMultiPart::FormDataType);
  QHttpPart file_part, select_part, print_part;

  {
	file_part.setHeader(
		QNetworkRequest::ContentDispositionHeader,
		"form-data; name=\"file\"; filename=\"" + file_name + '"');
	file_part.setHeader(QNetworkRequest::ContentTypeHeader,
						"application/octet-stream");
	file_part.setBody(data);

	select_part.setHeader(QNetworkRequest::ContentDispositionHeader,
						  "form-data; name=\"select\"");
	select_part.setBody(select ? "true" : "false");

	print_part.setHeader(QNetworkRequest::ContentDispositionHeader,
						 "form-data; name=\"print\"");
	print_part.setBody(print ? "true" : "false");
  }

  query->append(file_part);
  query->append(select_part);
  query->append(print_part);

  return query;
}

QHttpMultiPart *UploadFile::BuildFolderCreatePacket(QString folder_name,
													QString path) {
  QHttpMultiPart *query = new QHttpMultiPart(QHttpMultiPart::FormDataType);
  QHttpPart file_part, select_part;

  {
	file_part.setHeader(QNetworkRequest::ContentDispositionHeader,
						"form-data; name=\"foldername\"");
	file_part.setBody(folder_name.toUtf8());

	if (path.length()) {
	  select_part.setHeader(QNetworkRequest::ContentDispositionHeader,
							"form-data; name=\"path\"");
	  select_part.setBody(path.toUtf8());
	}
  }

  query->append(file_part);
  query->append(select_part);

  return query;
}

void UploadFile::OnReplyFinished(QNetworkReply *reply) {
  CheckStatusCodeAndResponse<Response>(reply);
}
}  // namespace commands
}  // namespace pi
}  // namespace c3picko
