#include "include/updater.h"
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QRegExp>
#include <QSettings>
#include <QThread>
#include <QTimer>
#include "include/process.h"
#include "include/version_manager.h"

namespace c3picko {
Updater::Updater(const QSettings& settings, Database& db)
    : db_(db),
      working_dir_(ResourcePath::fromSystemAbsolute(
          settings.value("working_dir").toString())),
      repo_url_(settings.value("repo_url").toString()),
      repo_branch_(settings.value("repo_branch").toString()),
      mng_(
          new VersionManager(working_dir_, repo_url_, repo_branch_, db, this)) {
  if (!db_.versions().exists(currentVersion().id()))
    db_.versions().add(currentVersion().id(), currentVersion());

  // TODO sanity check values
  if (!settings.contains("interval")) {
    qWarning() << "Updater interval not given, using default:"
               << defaultInterval << "s";
    interval_s_ = defaultInterval;
  } else
    interval_s_ = settings.value("interval").toInt();

  if (working_dir_.isEmpty())
    throw Exception("Updater: Working directory not set");

  if (!QDir(sourceDir().toSystemAbsolute()).exists())
    throw Exception("Source directory not found: " +
                    sourceDir().toSystemAbsolute());
  if (!QDir(buildDir().toSystemAbsolute()).exists())
    throw Exception("Build directory not found: " +
                    sourceDir().toSystemAbsolute());
  auto currbuild = buildDir(currentVersion().id()).toSystemAbsolute();
  if (!QDir(currbuild).exists())
    throw Exception("Build directory of current version not found: " +
                    currbuild);

  timer_ = new QTimer(this);
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
  qWarning() << "this is the new version";
}

Updater::~Updater() {
  stopSearch();
  // deleted by this
}

ResourcePath Updater::sourceDir() const { return working_dir_ + "source/"; }

ResourcePath Updater::buildDir(Version::ID id) const {
  auto prefix = working_dir_ + "builds/";
  auto ret = prefix + id;
  return ret;
}

Process* Updater::clone() {
  Process* git(
      Process::gitClone(repo_url_, sourceDir(), {"-n"}));  // -n for no checkout

  // connect(git, &Process::OnStarted,
  //	[this]() { qDebug() << "Cloning" << repo_url_ << "into" <<
  // sourceDir().toSystemAbsolute() << "..."; });
  connect(git, &Process::OnSuccess, []() { qDebug() << "Cloned"; });
  connect(git, &Process::OnFailure,
          [](QString output) { qDebug() << "Cloned error;" << output; });
  connect(git, &Process::OnFinished, git, &Process::deleteLater);

  return git;
}

Process* Updater::checkout(Version::ID hash) {
  Process* git(Process::gitCheckout(hash, sourceDir()));

  // connect(git, &Process::OnStarted, [hash]() { qDebug() << "Checkint out" <<
  // hash << "..."; });
  connect(git, &Process::OnSuccess, []() { qDebug() << "Checked out"; });
  connect(git, &Process::OnFailure,
          [](QString output) { qDebug() << "Check out error:" << output; });
  connect(git, &Process::OnFinished, git, &Process::deleteLater);

  return git;
}

void Updater::startSearch() {
  Q_ASSERT(timer_);
  timer_->start();
}

void Updater::stopSearch() {
  Q_ASSERT(timer_);
  timer_->stop();
}

void Updater::search() {
  timer_->stop();
  qDebug().nospace() << "Searching for updates... (thread="
                     << QThread::currentThreadId() << ")";

  Process* gitF = Process::gitFech(sourceDir(), {"origin"});
  Process* gitL = Process::gitLog(
      sourceDir(),
      {"--pretty=format:\"%H#%ad\"", "--date=rfc2822", "--max-count=5",
       currentVersion().id() + "...origin/" + repo_branch_});

  connect(gitF, &Process::OnSuccess, gitL, &Process::start);
  connect(gitF, &Process::OnFinished, gitF, &Process::deleteLater);
  connect(gitF, &Process::OnFailure, gitL, &Process::deleteLater);

  connect(gitL, &Process::OnSuccess, this, &Updater::logSuccess);
  connect(gitL, &Process::OnFailure, this, &Updater::logFailure);
  connect(gitL, &Process::OnFinished, this, [this]() { timer_->start(); });
  connect(gitL, &Process::OnFinished, gitL, &Process::deleteLater);

  gitF->start();
}

void Updater::logSuccess(QString output) {
  QStringList lines = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
  qDebug("origin/%s is %i ahead and 0 behind HEAD", qPrintable(repo_branch_),
         lines.size());
  std::stack<Version::ID> newest;

  for (QString line : lines) {
    QStringList splits = line.split('#');
    Version::ID id = splits[0];
    QDateTime date(QDateTime::fromString(splits[1], Qt::RFC2822Date));

    if (date.isNull()) {
      qWarning() << "Invalid date from 'git log':" << splits[0];
      continue;
    }
    if (!QRegExp("^[0-9a-f]{40}$")
             .exactMatch(id))  // match SHA-1 160 bit hashes
    {
      qWarning() << "Invalid id from 'git log':" << splits[1];
      continue;
    }

    if (!db_.versions().exists(id)) {
      qDebug() << "Commit" << id << "is new";
      // New versions dont have source or build directories yet
      db_.versions().add(id, Version(id, date));
      try {
        mng_->addVersion(id);
      } catch (std::exception const& e) {
        qCritical() << "VersionManager error:" << e.what();
        timer_->stop();
        return;
      } catch (...) {
        qCritical() << "VersionManager error:"
                    << "unknown";
        timer_->stop();
        return;
      }
    } else
      qWarning() << "Commit" << id << "is known";  // Should not happen, since
                                                   // we only only log the ones
                                                   // newer than HEAD
  }
}

void Updater::logFailure(QString output) {
  qCritical() << "Git log failed:" << output;
}

qint32 Updater::defaultInterval = 60;
bool Updater::defaultEnabled = false;
}  // namespace c3picko
