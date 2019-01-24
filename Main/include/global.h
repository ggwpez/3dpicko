#pragma once

#include <QString>
#include <QDir>
#include <QJsonValue>
#include <QDateTime>
#include <QVariant>
#include <QSettings>

class QSslConfiguration;
namespace c3picko {

QSslConfiguration* LoadSslConfig(QSettings* settings);

inline QDateTime parseDateTime(QJsonValue obj)
{
	return QDateTime::fromMSecsSinceEpoch(obj.toVariant().toLongLong());
}

inline QString Root()
{
	return "/home/vados/Code/Projects/3cpicko/GUI/";
}

inline QString Etc()
{
	return Root() + "etc/";
}

inline QString DocRoot()
{
	return Root() + "docroot/";
}

inline QString UploadFolderName()
{
	return "uploads";
}

inline QString UploadFolder()
{
	return DocRoot() + UploadFolderName() + "/";
}

inline void Setup()
{
	if (! QDir(UploadFolder()).exists())
		QDir().mkdir(UploadFolder());


}
} // namespace c3picko
