#include "pi_communicator.h"
#include "commands/all.h"
#include <QRandomGenerator64>
#include <QString>

namespace c3picko
{
namespace pi
{
	PiCommunicator::PiCommunicator(QString ip, ApiKey key, QObject* _parent) : QObject(_parent), printer_(ip, key, this) {}

	void PiCommunicator::Connect()
	{
		commands::Connection* cmd = new commands::Connection;

		connect(cmd, SIGNAL(OnStatusOk(int, Response*)), this, SIGNAL(OnConnected));
		connect(cmd, &Command::OnStatusErr, this /* ctx */,
				[this](QVariant code) { emit OnConnectionError("Wrong status code: " + code.toString()); });
		connect(cmd, SIGNAL(OnNetworkErr()), this, SIGNAL(OnNetworkErr()));
		// Delete
		connect(cmd, SIGNAL(OnFinished()), cmd, SLOT(deleteLater()));
	}

	void PiCommunicator::Disconnect() { ; }

	PrintJob* PiCommunicator::StartJob(GCode gcode)
	{
		PrintJob*  job  = new PrintJob;
		QByteArray data = gcode.join('\n').toUtf8();
		QString	name = gen_filename();

		commands::UploadFile* cmd = new commands::UploadFile(data, data::Location::LOCAL, name, true, true);

		QObject::connect(cmd, SIGNAL(OnStatusOk(int, Response*)), job, SIGNAL(OnUploaded()));
		QObject::connect(cmd, &Command::OnStatusErr, this, &PiCommands::OnStatusErr);
		QObject::connect(cmd, SIGNAL(OnNetworkErr(QString)), job, SIGNAL(OnError(QString)));
		QObject::connect(this, &PiCommands::done, &app, &QCoreApplication::quit);

		return job;
	}

	QString PiCommunicator::gen_filename() const
	{
		static QRandomGenerator64 gen(quint32(std::time(nullptr))); // FIXME
		QByteArray				  data(16, 0);
		gen.generate(data.begin(), data.end());

		return "_3cpicko-" + QString(data.toHex()) + ".gcode";
	}
}
} // namespace c3picko
