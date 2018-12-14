#include "datamodel/job_pause_action.h"

namespace c3picko
{
namespace pi
{
	namespace data
	{
		QString ToString(PauseAction val)
		{
			switch (val)
			{
				case PauseAction::PAUSE:
					return "pause";
				case PauseAction::TOGGLE:
					return "toggle";
				case PauseAction::RESUME:
					return "resume";
				default:
					Q_UNREACHABLE();
			}
		}
	}
}
}
