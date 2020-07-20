#ifndef CONNECTION_H
#define CONNECTION_H

#include <QPair>
#include <QSet>

#include "PiCommunicator/response.h"

namespace c3picko {
namespace pi {
namespace responses {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/connection.html#get--api-connection)
 * The Octoprint Documentation is very unclear here, what members to expect. I
 * looked
 * [here](https://github.com/foosel/OctoPrint/blob/master/src/octoprint/server/api/connection.py#L81)
 *
 */
struct ConnectionInfo : public Response {
  ConnectionInfo(QJsonObject const& obj);

  struct {
	/**
	 * @brief Octoprint does not clarify what options are available
	 */
	QString state;
	QString port;
	quint32 baudrate;
	QString printer_profile;
  } current;

  struct {
	QSet<QString> ports;
	QSet<quint32> baudrates;
	QSet<QPair<QString, QString>> printer_profiles;
	QString port_preference;
	quint32 baudrate_preference;
	QString printer_profile_preference;
	bool autoconnect;
  } options;
};
}  // namespace responses
}  // namespace pi
}  // namespace c3picko

#endif	// CONNECTION_H
