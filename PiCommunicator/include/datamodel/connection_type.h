#ifndef CONNECTION_TYPE_H
#define CONNECTION_TYPE_H

#include <QString>

namespace c3picko
{
namespace pi
{
	namespace data
	{
		/**
		 * @brief [API](http://docs.octoprint.org/en/master/api/connection.html#post--api-connection)
		 */
		enum class ConnectionType
		{
			CONNECT,
			DISCONNECT,
			FAKE_ACK
		};

		QString ToString(ConnectionType val);
	}
}
}

#endif // CONNECTION_TYPE_H
