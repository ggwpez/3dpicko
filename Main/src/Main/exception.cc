#include "Main/exception.h"
#include <string.h>

namespace c3picko {
Exception::Exception(const QString& what)
	: std::runtime_error(qPrintable(what)) {}

Exception::Exception(const QString& where, const QString& what)
	: std::runtime_error(qPrintable(where + ": " + what)) {}
}  // namespace c3picko
