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
QSslConfiguration* loadSslConfig(QSettings& settings);

namespace paths {
ResourcePath root();
ResourcePath etc();
/**
 * Document Root for the webserver.
 */
ResourcePath docRoot();
QString uploadFolderName();
ResourcePath uploadFolder();
ResourcePath reportFolder();
}  // namespace paths

QString searchConfigFile(QStringList args);
/**
 * @brief Must be called in the beginning. Sets up the paths and creates
 * necessary directories.
 * @return getConfig()
 */
QString setupGlobal(QCoreApplication* app);
void cleanupGlobal();

/**
 * @return Path to serverconfig.ini. Only works after Setup
 */
QString getConfigPath();
const Version& currentVersion();
QDateTime buildTime();
int getSubprocessTimeoutMs();
int getBacklogLength();

/**
 * @brief logTextColor
 * @param type Type of the log message
 * @return ANSI escape with text color for the specified message type
 */
QString consoleTextColor(QtMsgType type);

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

int exitCodeSuccess();

/**
 * Returns the default image extension for saving images.
 */
char const* defaultImageExtension();
}  // namespace c3picko
