#ifndef FILE_OPERATION_H
#define FILE_OPERATION_H

#include "command.h"
#include "datamodel/abridged_file_info.h"
#include "datamodel/location.h"

namespace c3picko
{
namespace pi
{
	namespace commands
	{
		/**
		 * [API](http://docs.octoprint.org/en/master/api/files.html#issue-a-file-command)
		 */
		class FileOperation : public Command
		{
			Q_OBJECT

			FileOperation() = delete;
			using Command::Command;

		  public:
			typedef responses::AbridgedFileInfo Response;

			static FileOperation* select(data::Location location, QString path, bool print = false);
			// Omitted
			// static FileOperation slice(data::Location location, QString path, QString slicer, QString gcode, );
			static FileOperation* copy(data::Location location, QString origin, QString destination);
			static FileOperation* move(data::Location location, QString origin, QString destination);

		  public slots:
			virtual void OnReplyFinished(QNetworkReply* reply) override;
		};
	} // namespace commands
} // namespace pi
} // namespace c3picko

#endif // FILE_OPERATION_H
