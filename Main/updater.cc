#include "include/updater.h"
#include <QDebug>
#include <QProcess>
#include <QRegExp>
#include <QSettings>
#include <QThread>
#include <QTimer>

namespace c3picko {
Updater::Updater(const QSettings& settings, Database& db) : db_(db) {
  worker_ = new QThread(this);
  // TODO sanity check values
  if (!settings.contains("interval")) {
    qWarning() << "Updater interval not given, using default:"
               << defaultInterval << "s";
    interval_s_ = defaultInterval;
  } else
    interval_s_ = settings.value("interval").toInt();

  timer_ = new QTimer(nullptr);
  timer_->moveToThread(worker_);
  timer_->setInterval(interval_s_ * 1000);
  connect(timer_, &QTimer::timeout, this, &Updater::search);

  if (!settings.contains("enabled")) {
    qWarning() << "Updater enabled not given, using default:" << defaultEnabled
               << "s";
    startSearch();
  } else if (settings.value("enabled").toBool())
    startSearch();
  else
    qInfo() << "Updater disabled";

  qDebug() << "Updater setup thread id =" << QThread::currentThreadId();
}

Updater::~Updater() {
  stopSearch();
  worker_->quit();
  worker_->wait(1000);
  // deleted by this
}

void Updater::startSearch() {
  Q_ASSERT(timer_);
  // Timer runs in different thread, we better use async calls
  QMetaObject::invokeMethod(timer_, "start");
  worker_->start();
}

void Updater::stopSearch() {
  Q_ASSERT(timer_);
  QMetaObject::invokeMethod(timer_, "stop");
}

void Updater::search() {
  qDebug() << "Updater search thread id =" << QThread::currentThreadId();

  QString git_path("git");
  QProcess git;
  git.setWorkingDirectory("/home/vados/Code/Projects/3cpicko");
  git.start(git_path, {"log", "--pretty=format:\"%H#%ad\"", "--date=rfc2822",
                       "--max-count=5"});

  if (!git.waitForStarted(3000))
    qCritical().nospace() << "Could not start git '" << git_path << "' "
                          << git.errorString();
  else if (!git.waitForFinished(3000))
    qCritical().nospace() << "Git timed out (path=" << git_path << ")";
  else if (git.exitStatus() != QProcess::NormalExit)
    qCritical().noquote() << "Git crashed (path=" << git_path
                          << "): " << git.readAllStandardError();
  else {
    QString output = git.readAllStandardOutput();
    QStringList lines =
        output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    qDebug("Detected last %i versions:", lines.size());

    for (QString line : lines) {
      QStringList splits = line.split('#');
      Version version(splits[0],
                      QDateTime::fromString(splits[1], Qt::RFC2822Date));
    }
  }
}

qint32 Updater::defaultInterval = 60;
bool Updater::defaultEnabled = false;
}  // namespace c3picko
