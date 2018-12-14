#include "datamodel/connection_type.h"

namespace c3picko
{
namespace pi
{
	namespace data
	{
		QString ToString(ConnectionType val)
		{
			if (val == ConnectionType::CONNECT)
				return "connect";
			if (val == ConnectionType::DISCONNECT)
				return "disconnect";
			if (val == ConnectionType::FAKE_ACK)
				return "fake_ack";
			else
				Q_UNREACHABLE();
		}
	}
}
}
