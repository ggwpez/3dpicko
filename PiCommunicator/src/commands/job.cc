#include "commands/job.h"

namespace c3picko
{
namespace pi
{
	namespace commands
	{

		Job::Job() : Command("job", {200}, HTTPType::GET) {}

		Job::Job(data::Action act) : Command("job", {{"command", ToString(act)}}, {204}, HTTPType::POST) {}

		Job::Job(data::PauseAction act) : Command("job", {{"command", "pause"}, {"action", ToString(act)}}, {204}, HTTPType::POST) {}

		void Job::OnReplyFinished(QNetworkReply* reply)
		{
			if (type_ == HTTPType::GET)
				CheckStatusCodeAndResponse<Response>(reply);
			else
				CheckStatusCode(reply);
		}
	}
}
}
