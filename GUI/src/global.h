#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QDir>

inline QString Root()
{
	return "../GUI/";
}

inline QString Etc()
{
	return Root() +"etc/";
}

inline QString DocRoot()
{
	return Root() +"docroot/";
}

inline QString UploadFolderName()
{
	return "images";
}

inline QString UploadFolder()
{
	return DocRoot() +UploadFolderName() +"/";
}

inline void Setup()
{
	if (! QDir(UploadFolder()).exists())
		QDir().mkdir(UploadFolder());
}

#endif // GLOBAL_H
