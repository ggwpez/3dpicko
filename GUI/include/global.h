#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QDir>

namespace c3picko {

inline QString Root()
{
    return "../../../3cpicko/GUI/";
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
	return "images";
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
