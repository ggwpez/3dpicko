#include "Main/logger.h"

#include <QMutexLocker>

#include "Main/global.h"

namespace c3picko {
Logger::Logger(int backlog_length)
	: backlog_length_(backlog_length), lock_(QMutex::Recursive) {}

Logger* c3picko::Logger::instance() {
  static Logger* obj = new Logger;
  return obj;
}

void Logger::setBacklogLength(int backlog_length) {
  QMutexLocker guard(&lock_);

  backlog_length_ = backlog_length;
  normalise();
}

void Logger::normalise() {
  QMutexLocker guard(&lock_);
  int diff = backlog_.size() - backlog_length_;

  if (diff > 0) backlog_.erase(backlog_.begin(), backlog_.begin() + diff);
}

void Logger::log(QString line) {
  QMutexLocker guard(&lock_);

  backlog_.append(line);
  normalise();

  guard.unlock();  // NOTE not sure if needed
  emit OnNewLine(line);
}

void Logger::log(QStringList lines) {
  QMutexLocker guard(&lock_);

  backlog_.append(lines);
  normalise();

  guard.unlock();
  for (QString line : lines) emit OnNewLine(line);
}
}  // namespace c3picko
