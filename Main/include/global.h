#pragma once

#include <QDateTime>
#include <QDir>
#include <QJsonValue>
#include <QSettings>
#include <QString>
#include <QVariant>

class QCoreApplication;
class QSslConfiguration;
namespace c3picko {
class ResourcePath;

QSslConfiguration* LoadSslConfig(QSettings& settings);

ResourcePath Root();
ResourcePath Etc();
ResourcePath DocRoot();
QString UploadFolderName();
ResourcePath UploadFolder();

QString searchConfigFile(int argc, char** argv);
void Setup(QCoreApplication* app, QString ini_file_path, QSettings& settings);

/**
 * @brief String format for formatting QDateTime.
 * See https://doc.qt.io/qt-5/qdatetime.html#toString
 */
QString dateTimeFormat();

/**
 * @brief Exit code indicating restart of the application.
 * I would use a macro aka EXIT_RESTART but no macros since google style
 * guide...
 */
int exitRestart();

char const* defaultImageExtension();
}  // namespace c3picko
