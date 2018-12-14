#ifndef OCTOPRINT_H_
#define OCTOPRINT_H_

#include <QNetworkAccessManager>
#include <QObject>

#include "apikey.h"
#include "command.h"

namespace c3picko
{
namespace pi
{
	/**
	 * @brief API for OctoPrint printer
	 */
	class OctoPrint : public QObject
	{
		Q_OBJECT

	  public:
		OctoPrint(QString ip, ApiKey key, QObject* _parent = nullptr);

	  public slots:
		void SendCommand(Command* cmd);

	  signals:

	  private:
		QString				   ip_;
		ApiKey				   apikey_;
		QNetworkAccessManager* network_;
	};
}
} // namespace c3picko

// TODO QNetworkAccessManager::networkAccessibleChanged()
#endif // OCTOPRINT_H_
