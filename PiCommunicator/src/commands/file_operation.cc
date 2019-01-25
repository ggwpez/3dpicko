#include "commands/file_operation.h"

namespace c3picko
{
namespace pi
{
	namespace commands
	{
		FileOperation* FileOperation::select(data::Location location, QString path, bool print)
		{
			// TODO API says 200 on one site and 204 on the other
			return new FileOperation("files/" + data::ToString(location) + '/' + path,
									 {{"command", "select"}, {"print", print ? "true" : "false"}}, {200, 204}, HTTPType::POST);
		}

		FileOperation* FileOperation::copy(data::Location location, QString origin, QString destination)
		{
			return new FileOperation("files/" + data::ToString(location) + '/' + origin,
									 {{"command", "copy"}, {"destination", destination}}, {201}, HTTPType::POST);
		}

		FileOperation* FileOperation::move(data::Location location, QString origin, QString destination)
		{
			return new FileOperation("files/" + data::ToString(location) + '/' + origin,
									 {{"command", "move"}, {"destination", destination}}, {201}, HTTPType::POST);
		}

		void FileOperation::OnReplyFinished(QNetworkReply* reply)
		{
			if (!status_ok_.contains(200))
				CheckStatusCodeAndResponse<Response>(reply);
			else
				CheckStatusCode(reply);
		}
	}
}
}
