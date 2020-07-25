#pragma once

#include <QString>

namespace c3picko {
enum class ProfileType { PRINTER, PLATE, SOCKET, OCTOPRINT };
QString profileToString(ProfileType);
ProfileType profileFromString(QString);
}  // namespace c3picko
