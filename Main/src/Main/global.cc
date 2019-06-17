#include "ImageRecognition/algorithms/helper.h"
#include "Main/exception.h"
#include "Main/logger.h"
#include "Main/resource_path.h"
#include "Main/version.h"
#include <QCoreApplication>
#include <QSslConfiguration>
#include <QSslKey>
#include <QtDebug>

#ifdef Q_OS_UNIX
#include "Main/signal_daemon.h"
#endif

namespace c3picko
{
static ResourcePath		  root_path;
static ResourcePath		  doc_root_path;
static ResourcePath		  report_path, upload_path;
static QSslConfiguration* LoadSsl(QString key, QString cert)
{
	QSslConfiguration* ssl = new QSslConfiguration;

	QFile certFile(cert);
	QFile keyFile(key);

	if (!certFile.open(QIODevice::ReadOnly))
		throw Exception("Certificate file: " + cert + ": " + certFile.errorString());
	if (!keyFile.open(QIODevice::ReadOnly))
		throw Exception("Key file: " + key + ": " + keyFile.errorString());

	QSslCertificate certificate(&certFile, QSsl::Pem);
	QSslKey			sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);

	ssl->setPeerVerifyMode(QSslSocket::VerifyNone);
	ssl->setLocalCertificate(certificate);
	ssl->setPrivateKey(sslKey);
	ssl->setProtocol(QSsl::SecureProtocols); // TODO

	return ssl;
}

QSslConfiguration* loadSslConfig(QSettings& settings)
{
	if (settings.value("enabled", false).toBool())
	{
		QString cert(settings.value("certificate").toString());
		QString key(settings.value("key").toString());
		// throws
		QSslConfiguration* ssl = LoadSsl(key, cert);

		qDebug() << "SSL enabled";
		return ssl;
	}
	else
	{
		qDebug() << "SSL disabled";
		return nullptr;
	}
}
namespace paths
{
	ResourcePath root() { return root_path; }
	ResourcePath etc() { return root() + "etc/"; }
	ResourcePath docRoot() { return doc_root_path; }
	QString		 uploadFolderName() { return "uploads"; }
	ResourcePath uploadFolder() { return docRoot() + uploadFolderName() + "/"; }
	ResourcePath reportFolder() { return docRoot() + "reports/"; }
} // namespace paths
/**
 * Search the configuration file.
 * Aborts the application if not found.
 * @return The valid filename
 */
QString searchConfigFile(QStringList args)
{
	QFile   file;
	QString path = QCoreApplication::applicationDirPath() + "/serverconfig.ini";
	if (args.size() > 1)
		path = args[1];
	file.setFileName(path);

	QFileInfo info(file);
	if (file.exists())
	{
		QString configFileName = QDir(file.fileName()).canonicalPath();
		return configFileName;
	}
	else
	{
		qFatal("Ini file not found, pass path as first argument.");
		qApp->exit(1);
	}
	return "";
}

static void setupSignals(QCoreApplication* app)
{
#ifdef Q_OS_UNIX
	SignalDaemon* sigwatch = new SignalDaemon(app);
	sigwatch->registerSignals({SIGABRT, SIGALRM, SIGFPE, SIGHUP, SIGILL, SIGINT, SIGPIPE, SIGQUIT, SIGSEGV, SIGTERM, SIGUSR1, SIGUSR2});

	QObject::connect(sigwatch, &SignalDaemon::OnSignal, [&app](int signum) {
		qWarning("Shutdown by signal: %s", ::strsignal(signum));
		if (signum == SIGUSR1)
			qApp->exit(exitCodeSuccess());
		else if (signum == SIGUSR2)
			qApp->exit(exitCodeHardRestart());
		else
			app->quit();
	});
#else
	qInfo() << "UNIX Signal Setup skipped";
#endif
}

static void qtTypeSetup() { QMetaType::registerConverter<math::Range<double>, QString>(math::rangeToString); }

static int	 subprocess_timeout_ms;
static QString ini_file_path;
QString		   setupGlobal(QCoreApplication* app)
{
	setupSignals(app);
	qtTypeSetup();
	ini_file_path = searchConfigFile(app->arguments());
	QSettings settings(ini_file_path, QSettings::IniFormat);
	settings.beginGroup("global");

	QString path = QFileInfo(ini_file_path).absolutePath() + "/" + settings.value("root").toString();
	if (!path.endsWith('/'))
		path += '/';

	root_path	 = ResourcePath::fromSystemAbsolute(path);
	doc_root_path = root_path + settings.value("docroot").toString();
	upload_path   = root_path + settings.value("uploads").toString();
	report_path   = root_path + settings.value("reports").toString();

	if (!paths::root().exists() || !paths::root().isDir())
		throw Exception("Root '" + paths::root().toSystemAbsolute() + "' must be a valid directory");
	if (!paths::docRoot().exists() || !paths::docRoot().isDir())
		throw Exception("DocRoot '" + paths::docRoot().toSystemAbsolute() + "' must be a valid directory");

	// Create "uploads" folder
	if (!QDir(paths::uploadFolder().toSystemAbsolute()).exists())
		QDir().mkdir(paths::uploadFolder().toSystemAbsolute());
	// Create "report" folder
	if (!QDir(paths::reportFolder().toSystemAbsolute()).exists())
		QDir().mkdir(paths::reportFolder().toSystemAbsolute());

	subprocess_timeout_ms = settings.value("subprocess_timeout_s", 60 * 1000).toInt() * 1000;
	int backlog_length	= settings.value("backlog_length", 1000).toInt();
	Logger::instance()->setBacklogLength(backlog_length);

	qDebug().noquote() << "Version:" << currentVersion().id() << "built on" << currentVersion().date().toString(dateTimeFormat());

	return ini_file_path;
}

QString consoleTextColor(QtMsgType type)
{
	switch (type)
	{
		case QtMsgType::QtInfoMsg:
			return "";
		case QtMsgType::QtDebugMsg:
			return "";
		case QtMsgType::QtWarningMsg:
			return "\x1B[33m"; // Orange
		case QtMsgType::QtCriticalMsg:
			return "\x1B[31m";		// Red
		case QtMsgType::QtFatalMsg: // is the same as QtSystemMsg
			return "\x1B[31;1m";	// Red, Fat
		default:
			Q_UNREACHABLE();
	}
}

QString htmlTextColor(QtMsgType type)
{
	QString clr = "green";

	switch (type)
	{
		case QtMsgType::QtWarningMsg:
			clr = "orange"; // Orange
			break;
		case QtMsgType::QtCriticalMsg:
		case QtMsgType::QtFatalMsg: // is the same as QtSystemMsg
			clr = "red";			// Red
			break;
		default:
			break;
	}

	return "<font color='" + clr + "'>";
}

static std::function<void(QString)> messageHandler;
static void							handleQtMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QString message = qFormatLogMessage(type, context, msg);
	QString time	= "[ " + QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate) + " ] ";

	QString console_string = time + consoleTextColor(type) + message + "\033[0m";
	QString html_string	= time + htmlTextColor(type) + message + "</font>";

	// TODO bad style to write everything to stderr
#if defined(Q_OS_LINUX) || defined(C3PICKO_TEXT_COLORS_OFF)
	fprintf(stderr, "%s\n", qPrintable(console_string));
#else
	fprintf(stderr, "%s%s\n", qPrintable(time), qPrintable(message));
#endif

	Logger::instance()->log(html_string);
}

void setMessageHandler(const std::function<void(QString)>& handler) { messageHandler = handler; }

void startLog() { qInstallMessageHandler(handleQtMessage); }

void stopLog() { qInstallMessageHandler(nullptr); }

QString dateTimeFormat() { return "dd.MM.yy HH:mm:ss"; }

int exitCodeSoftRestart() { return 123; }
int exitCodeHardRestart() { return 101; }
int exitCodeSuccess() { return 0; }
int exitCodeError() { return 1; }

const char* defaultImageExtension() { return ".png"; }

QString getConfigPath()
{
	if (ini_file_path.isEmpty())
		throw Exception("Setup must be called before getConfig()");

	return ini_file_path;
}

Version const& currentVersion()
{
	static Version current(GIT_HASH, QDateTime::fromString(GIT_DATE, Qt::RFC2822Date));

	return current;
}

int getSubprocessTimeoutMs() { return subprocess_timeout_ms; }

void cleanupGlobal()
{
	// FIXME QMetaType::unregisterConverterFunction<math::Range<double>, QString>(math::rangeToString);
}
} // namespace c3picko
