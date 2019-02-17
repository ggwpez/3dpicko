#pragma once

#include <QObject>
#include <QSet>
#include <signal.h>
class QSocketNotifier;

namespace c3picko
{
/**
 * @brief Class for intercepting UNIX signals and making them accessible for Qt
 */
class SignalDaemon : public QObject
{
	Q_OBJECT

  public:
	SignalDaemon(QObject* _parent = nullptr);

	// Unix signal handler
	int registerSignal(int signal);
	int registerSignals(QVector<int> signal);

	static void signalProxy(int signal);

  private slots:
	void socketNotify(int socket_fd);

  signals:
	void OnSignal(int signal);

  private:
	static int socketpair_[2];

	QSocketNotifier* notifier_;
	QSet<int>		 registered_;
};
}
