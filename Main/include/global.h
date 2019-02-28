#pragma once

#include <QDateTime>
#include <QDir>
#include <QJsonValue>
#include <QSettings>
#include <QString>
#include <QVariant>

class QSslConfiguration;
namespace c3picko {

QSslConfiguration* LoadSslConfig(QSettings* settings);

inline QDateTime parseDateTime(QJsonValue obj) {
  return QDateTime::fromMSecsSinceEpoch(obj.toVariant().toLongLong());
}

inline QString Root() { return "/home/vados/Code/Projects/3cpicko/GUI/"; }

inline QString Etc() { return Root() + "etc/"; }

inline QString DocRoot() { return Root() + "docroot/"; }

inline QString UploadFolderName() { return "uploads"; }

inline QString UploadFolder() { return DocRoot() + UploadFolderName() + "/"; }

inline void Setup() {
  if (!QDir(UploadFolder()).exists()) QDir().mkdir(UploadFolder());
}

/**
 * @brief String format for formatting QDateTime.
 * See https://doc.qt.io/qt-5/qdatetime.html#toString
 */
inline QString dateTimeFormat() { return "dd.MM.yy HH:mm"; }

inline char const* defaultImageExtension() { return "jpg"; }
}  // namespace c3picko
