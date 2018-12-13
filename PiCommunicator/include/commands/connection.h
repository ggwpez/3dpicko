#ifndef CONNECT_H_
#define CONNECT_H_

#include "command.h"
#include "datamodel/connection_type.h"
#include "responses/connection_info.h"

namespace c3picko
{
namespace pi
{
	namespace commands
	{
		/**
		 * @brief Allows to send arbitraty commands to the printer.
		 *
		 * Use with care, this can break current jobs, since the commands are executed
		 * immediately.
		 */
		class Connection : public Command
		{
			Q_OBJECT

		  public:
			typedef responses::ConnectionInfo Response;
			/**
			 * @brief Connection Get connection settings
			 */
			Connection();
			Connection(data::ConnectionType type);

			/**
			 * @brief Connect with arguments. If you dont want arguments use the ConnectionType overload
			 * [API](http://docs.octoprint.org/en/master/api/connection.html#get-connection-settings)
			 */
			Connection(QString port, qint32 baudrate = -1, QString printer_profile = "", bool save = false, bool autoconnect = false);

		public slots:
		  virtual void OnReplyFinished(QNetworkReply* reply) override;

		  private:
			QJsonObject CreateConnectJson(QString port, qint32 baudrate, QString printer_profile, bool safe, bool autoconnect);
		};
	} // namespace commands
} // namespace pi
} // namespace c3picko

#endif // CONNECT_H
