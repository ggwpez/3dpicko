#ifndef JOB_H
#define JOB_H

#include "command.h"
#include "datamodel/job_action.h"
#include "datamodel/job_pause_action.h"
#include "responses/job_info.h"

namespace c3picko
{
namespace pi
{
	namespace commands
	{
		/**
		 * @brief Job controll for the currently running job on OctoPrint
		 */
		class Job : public Command
		{
			Q_OBJECT

		  public:
			typedef responses::JobInfo Response;

			/**
			 * @brief Retrive information about the current job
			 */
			Job();
			Job(data::Action act);
			Job(data::PauseAction act);

		  public slots:
			virtual void OnReplyFinished(QNetworkReply* reply) override;
		};
	} // namespace commands
} // namespace pi
} // namespace c3picko

#endif // JOB_H
