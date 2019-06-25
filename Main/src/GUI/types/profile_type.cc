#include "GUI/types/profile_type.h"
#include <QtGlobal>
#include "Main/exception.h"

namespace c3picko {
QString profileToString(ProfileType val) {
  switch (val) {
	case ProfileType::OCTOPRINT:
	  return "octoprint-profile";
	case ProfileType::PLATE:
	  return "plate-profile";
	case ProfileType::PRINTER:
	  return "printer-profile";
	case ProfileType::SOCKET:
	  return "socket-profile";
  }

  throw Exception("Unknown profile type: " + QString::number(int(val)));
}

ProfileType profileFromString(QString val) {
  if (val == "octoprint-profile") return ProfileType::OCTOPRINT;
  if (val == "plate-profile") return ProfileType::PLATE;
  if (val == "printer-profile") return ProfileType::PRINTER;
  if (val == "socket-profile") return ProfileType::SOCKET;

  throw Exception("Unknown profile type: " + val);
}

}  // namespace c3picko
