#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QDir>
#include <QJsonValue>
#include <QDateTime>
#include <QVariant>

namespace c3picko {

inline QDateTime parseDateTime(QJsonValue obj)
{
	return QDateTime::fromMSecsSinceEpoch(obj.toVariant().toLongLong());
}

inline QString Root()
{
    return "../../GUI/";
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
#endif // GLOBAL_H
