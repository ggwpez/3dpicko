#ifndef GLOBAL_H
#define GLOBAL_H

#include <QDateTime>
#include <QDir>
#include <QJsonValue>
#include <QString>
#include <QVariant>

namespace c3picko {

inline QDateTime parseDateTime(QJsonValue obj) {
  return QDateTime::fromMSecsSinceEpoch(obj.toVariant().toLongLong());
}

inline QString Root() { return ""; }

inline QString Etc() { return Root() + "etc/"; }

inline QString DocRoot() { return Root() + "docroot/"; }

inline QString UploadFolderName() { return "uploads"; }

inline QString UploadFolder() { return DocRoot() + UploadFolderName() + "/"; }

inline void Setup() {
  if (!QDir(UploadFolder()).exists())
    QDir().mkdir(UploadFolder());
}
} // namespace c3picko
#endif // GLOBAL_H
