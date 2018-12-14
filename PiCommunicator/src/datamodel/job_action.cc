#include "datamodel/job_action.h"

namespace c3picko
{
namespace pi
{
	namespace data
	{
		QString ToString(Action val)
		{
			switch (val)
			{
				case Action::START:
					return "start";
				case Action::CANCEL:
					return "cancel";
				case Action::RESTART:
					return "restart";
				default:
					Q_UNREACHABLE();
			}
		}
	}
}
}
