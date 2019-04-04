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
class Version;
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
ResourcePath updateFolder();

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
QString getConfigPath();
const Version& currentVersion();
int getSubprocessTimeoutMs();

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
 * @brief Restarts the QCoreApplication but not the process itself.
 * Static (global) data stays the same.
 * @return 123
 */
int exitCodeSoftRestart();
/**
 * @brief Stops the process and indicates that start.sh should restart the
 * program.
 * @return 101
 */
int exitCodeHardRestart();

char const* defaultImageExtension();
}  // namespace c3picko
