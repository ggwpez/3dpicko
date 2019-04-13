#include "include/autosaver.h"
#include <QDebug>
#include <QRegularExpression>
#include <QSettings>
#include <QTimer>
#include "include/exception.h"

namespace c3picko {
Autosaver::Autosaver(const QSettings& settings, QObject* _parent)
    : QObject(_parent),
      interval_ms_(0),
      enabled_(false),
      data_changed_(false),
      timer_(nullptr) {
  if (!settings.contains("interval"))
    throw Exception("Autosave missing setting: interval");
  if (!settings.contains("enabled"))
    throw Exception("Autosave missing setting: enabled");

  QString str = settings.value("interval").toString().trimmed().toLower();
  int index = str.lastIndexOf(QRegularExpression("\\d"));
  QString number = str.left(index + 1),
          unit = str.right(str.size() - index - 1);

  bool ok = false;
  double v = number.toDouble(&ok);
  if (!ok) throw Exception("Could not parse number: " + number);
  if (v >= std::numeric_limits<decltype(interval_ms_)>::max())
    throw Exception("Interval too big: " + QString::number(v));

  if (unit == "s" || unit == "sec")
    interval_ms_ = qRound(v * 1000);
  else if (unit == "m" || unit == "min")
    interval_ms_ = qRound(v * 1000 * 60);
  else if (unit == "h" || unit == "hour")
    interval_ms_ = qRound(v * 1000 * 60 * 60);
  else if (unit == "d" || unit == "day")
    interval_ms_ = qRound(v * 1000 * 60 * 60 * 24);
  else
    throw Exception("Could not parse unit: " + unit);

  if ((enabled_ = settings.value("enabled").toBool())) {
    timer_ = new QTimer(this);
    timer_->setInterval(interval_ms_);
    connect(timer_, &QTimer::timeout, this, &Autosaver::tick);
  }
}

void Autosaver::start() {
  if (!enabled_) return;

  resetDataChanged();
  timer_->start();
}

void Autosaver::tick() {
  if (data_changed_) {
    resetDataChanged();
    emit OnAutosaveNeeded();
  } else
    emit OnAutosaveSkipp();
}

void Autosaver::stop() {
  if (!enabled_) return;

  timer_->stop();
}

void Autosaver::dataChanged() {
  if (!enabled_) return;

  data_changed_ = true;
}

void Autosaver::resetDataChanged() { data_changed_ = false; }
}  // namespace c3picko
