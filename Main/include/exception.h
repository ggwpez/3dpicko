#pragma once

#include <QException>

namespace c3picko {
class Exception : public std::runtime_error {
public:
  Exception(const QString &what);
  Exception(QString const &where, QString const &what);
};
} // namespace c3picko
