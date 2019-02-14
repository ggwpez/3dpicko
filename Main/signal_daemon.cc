#include "include/signal_daemon.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSocketNotifier>

#include <iostream>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

namespace c3picko
{
SignalDaemon::SignalDaemon(QObject* _parent) : QObject(_parent)
{
	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, socketpair_))
		qFatal("Couldn't create HUP socketpair");

	notifier_ = new QSocketNotifier(socketpair_[1], QSocketNotifier::Read, this);
	connect(notifier_, SIGNAL(activated(int)), this, SLOT(socketNotify(int)));
	notifier_->setEnabled(true); // dont forget this…
}

int SignalDaemon::registerSignal(int signal)
{
	if (registered_.contains(signal))
	{
		qWarning() << "Cant register signal" << signal << "twice";
		return 1;
	}

	struct sigaction sa;
	sa.sa_handler = SignalDaemon::signalProxy;
	sa.sa_flags   = 0;
	::sigemptyset(&sa.sa_mask);
	sa.sa_flags |= SA_RESTART;

	if (::sigaction(signal, &sa, nullptr))
	{
		qWarning() << "Cant register signal" << signal << "(sigaction=" << ::strerror(errno) << ")";
		return 1;
	}
	else
	{
		registered_.insert(signal);
		return 0;
	}
}

int SignalDaemon::registerSignals(QVector<int> signal)
{
	for (int sig : signal)
	{
		if (registerSignal(sig))
			return 1;
	}

	return 0;
}

void SignalDaemon::signalProxy(int signal)
{
	// Writing to the socket will wake up the QSocketNotifier
	::write(socketpair_[0], &signal, sizeof(signal));
}

void SignalDaemon::socketNotify(int socket_fd)
{
	int signal;
	::read(socket_fd, &signal, sizeof(signal));

	emit OnSignal(signal);
}

int SignalDaemon::socketpair_[2];
}
