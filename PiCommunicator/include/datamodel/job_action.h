#ifndef JOB_ACTION_H
#define JOB_ACTION_H

#include <QString>

namespace c3picko
{
namespace pi
{
	namespace data
	{
		///
		/// [API](http://docs.octoprint.org/en/master/api/job.html#post--api-job)
		///
		enum class Action
		{
			START,
			CANCEL,
			RESTART
		};

		QString ToString(Action val);
	} // namespace data
} // namespace commands
} // namespace c3picko

#endif // JOB_ACTION_H
