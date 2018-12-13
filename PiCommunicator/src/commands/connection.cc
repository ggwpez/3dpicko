#include "commands/connection.h"

namespace c3picko
{
namespace pi
{
	namespace commands
	{
		Connection::Connection() : Command("connection", {200}, HTTPType::GET) {}

		Connection::Connection(data::ConnectionType type) : Command("connection", {{"command", data::ToString(type)}}, {204}, HTTPType::POST)
		{
		}

		Connection::Connection(QString port, qint32 baudrate, QString printer_profile, bool save, bool autoconnect)
			: Command("connect", CreateConnectJson(port, baudrate, printer_profile, save, autoconnect), {204}, HTTPType::POST)
		{
		}

		void Connection::OnReplyFinished(QNetworkReply* reply)
		{
			if (type_ ==  HTTPType::GET) // Are we waiting for a response ?
				CheckStatusCodeAndResponse<Response>(reply);
			else if (type_ == HTTPType::POST) // No response in POST
				CheckStatusCode(reply, nullptr);
			else
				Q_UNREACHABLE();
		}

		QJsonObject Connection::CreateConnectJson(QString port, qint32 baudrate, QString printer_profile, bool save, bool autoconnect)
		{
			QJsonObject ret;
			ret["command"] = data::ToString(data::ConnectionType::CONNECT);

			if (port.length())
				ret["port"] = port;
			if (baudrate > 0)
				ret["baudrate"] = baudrate;
			if (printer_profile.length())
				ret["printerProfile"] = printer_profile;
			if (save)
				ret["save"] = true;
			if (autoconnect)
				ret["autoconnect"] = true;

			return ret;
		}
	}
}
}
