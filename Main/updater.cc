#include "include/updater.h"
#include <QDebug>
#include <QSettings>
#include <QTimer>

namespace c3picko {
Updater::Updater(const QSettings& settings) {
  // TODO sanity check
  if (!settings.contains("interval")) {
    qWarning() << "Updater interval not given, using default:"
               << defaultInterval << "s";
    interval_s_ = defaultInterval;
  } else
    interval_s_ = settings.value("interval").toInt();

  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &Updater::search);
  timer_->setInterval(interval_s_ * 1000);

  if (!settings.contains("enabled")) {
    qWarning() << "Updater enabled not given, using default:" << defaultEnabled
               << "s";
    startSearch();
  } else if (settings.value("enabled").toBool())
    startSearch();
  else
    qInfo() << "Updater disabled";
}

void Updater::startSearch() {
  Q_ASSERT(timer_);
  timer_->start(0);
}

void Updater::endSearch() {
  Q_ASSERT(timer_);
  timer_->stop();
}

void Updater::search() {}

qint32 Updater::defaultInterval = 60;
bool Updater::defaultEnabled = false;
}  // namespace c3picko
