#pragma once

#include <QDateTime>
#include <QDir>
#include <QJsonValue>
#include <QSettings>
#include <QString>
#include <QVariant>
#include <functional>

class QCoreApplication;
class QSslConfiguration;
namespace c3picko {
class ResourcePath;
/**
 * @return Throws on error.
 */
QSslConfiguration* LoadSslConfig(QSettings& settings);

ResourcePath Root();
ResourcePath Etc();
ResourcePath DocRoot();
QString UploadFolderName();
ResourcePath UploadFolder();
ResourcePath reportFolder();

QString searchConfigFile(QStringList args);
/**
 * @brief Must be called in the beginning. Sets up the paths and creates
 * necessary directories.
 * @return getConfig()
 */
QString Setup(QCoreApplication* app);

/**
 * @return Path to serverconfig.ini. Only works after Setup
 */
QString getConfig();

/**
 * @brief logTextColor
 * @param type Type of the log message
 * @return ANSI escape with text color for the specified message type
 */
QString logTextColor(QtMsgType type);

void startLog();
void stopLog();
void setMessageHandler(std::function<void(QString)> const& handler);

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
int exitCodeRestart();

char const* defaultImageExtension();
}  // namespace c3picko
