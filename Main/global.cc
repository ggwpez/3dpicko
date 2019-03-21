#include "include/exception.h"
#include "include/resource_path.h"
#include <QCoreApplication>
#include <QSslConfiguration>
#include <QSslKey>
#include <QtDebug>

#ifdef Q_OS_UNIX
#include "include/signal_daemon.h"
#endif

namespace c3picko {
static ResourcePath root_path;
QSslConfiguration *LoadSslConfig(QSettings &settings) {
  QSslConfiguration ssl;

  QString cert(settings.value("certificate").toString());
  QString key(settings.value("key").toString());

  QFile certFile(cert);
  QFile keyFile(key);
  if (!certFile.open(QIODevice::ReadOnly)) {
    qCritical() << "Certificate file:" << cert << ":" << certFile.errorString();
    return nullptr;
  }
  if (!keyFile.open(QIODevice::ReadOnly)) {
    qCritical() << "Key file:" << key << ":" << keyFile.errorString();
    return nullptr;
  }

  QSslCertificate certificate(&certFile, QSsl::Pem);
  QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
  ssl.setPeerVerifyMode(QSslSocket::VerifyNone);
  ssl.setLocalCertificate(certificate);
  ssl.setPrivateKey(sslKey);
  ssl.setProtocol(QSsl::SecureProtocols); // TODO

  return new QSslConfiguration(ssl);
}

ResourcePath Root() { return root_path; }

ResourcePath Etc() { return Root() + "etc/"; }

ResourcePath DocRoot() { return Root() + "docroot/"; }

QString UploadFolderName() { return "uploads"; }

ResourcePath UploadFolder() { return DocRoot() + UploadFolderName() + "/"; }

/**
 * Search the configuration file.
 * Aborts the application if not found.
 * @return The valid filename
 */
QString searchConfigFile(int argc, char **argv) {
  QFile file;
  QString path = QCoreApplication::applicationDirPath() + "/serverconfig.ini";
  if (argc > 1)
    path = argv[1];
  file.setFileName(path);

  QFileInfo info(file);
  if (file.exists()) {
    QString configFileName = QDir(file.fileName()).canonicalPath();
    qDebug("using config file %s", qPrintable(configFileName));
    return configFileName;
  } else {
    qFatal("Ini file not found, pass path to ini file as first argument.");
    qApp->exit(1);
  }
  return "";
}

static void setupSignals(QCoreApplication *app) {
#ifdef Q_OS_UNIX
  SignalDaemon *sigwatch = new SignalDaemon(app);
  sigwatch->registerSignals({SIGABRT, SIGALRM, SIGFPE, SIGHUP, SIGILL, SIGINT,
                             SIGPIPE, SIGQUIT, SIGSEGV, SIGTERM, SIGUSR1,
                             SIGUSR2});

  QObject::connect(sigwatch, &SignalDaemon::OnSignal, [&app](int signum) {
    qWarning("Shutdown by signal: %s", ::strsignal(signum));
    app->quit();
  });
#else
  qDebug() << "UNIX Signal Setup skipped";
#endif
}

void Setup(QCoreApplication *app, QString ini_file_path, QSettings &settings) {
  setupSignals(app);

  QString path = QFileInfo(ini_file_path).absolutePath() + "/" +
                 settings.value("root").toString();
  if (!path.endsWith('/'))
    path += '/';
  root_path = ResourcePath::fromSystemAbsolute(path);

  if (!Root().exists() || !Root().isDir())
    throw Exception("Root '" + Root().toSystemAbsolute() +
                    "' must be a valid directory");
  if (!DocRoot().exists() || !DocRoot().isDir())
    throw Exception("DocRoot '" + DocRoot().toSystemAbsolute() +
                    "' must be a valid directory");

  // Create "uploads" folder
  if (!QDir(UploadFolder().toSystemAbsolute()).exists())
    QDir().mkdir(UploadFolder().toSystemAbsolute());
}

QString dateTimeFormat() { return "dd.MM.yy HH:mm"; }

int exitRestart() { return 123; }

const char *defaultImageExtension() { return "jpg"; }

} // namespace c3picko
