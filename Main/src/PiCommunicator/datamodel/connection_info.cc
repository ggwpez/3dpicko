#include "PiCommunicator/responses/connection_info.h"

#include <QJsonArray>

namespace c3picko {
namespace pi {
namespace responses {
ConnectionInfo::ConnectionInfo(const QJsonObject& obj) : Response(obj) {
  QJsonObject current_json = obj["current"].toObject(),
			  options_json = obj["options"].toObject();

  current.state = current_json["state"].toString();
  current.port = current_json["port"].toString();
  current.baudrate = current_json["baudrate"].toInt();
  current.printer_profile = current_json["printerProfile"].toString();

  for (auto port : options_json["ports"].toArray())
	options.ports.insert(port.toString());

  for (auto rate : options_json["baudrates"].toArray())
	options.baudrates.insert(rate.toInt());

  for (auto profile : options_json["printerProfiles"].toArray())
	options.printer_profiles.insert(
		{profile.toObject()["name"].toString(),
		 profile.toObject()["id"].toString()});	 // QtJson is dumb

  options.port_preference = options_json["portreference"].toString();
  options.baudrate_preference = options_json["baudratePreference"].toInt();
  options.printer_profile_preference =
	  options_json["printerProfilePreference"].toString();
  options.autoconnect = options_json["autoconnect"].toBool();
}
}  // namespace responses
}  // namespace pi
}  // namespace c3picko
