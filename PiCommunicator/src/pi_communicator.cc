#include "pi_communicator.h"

namespace c3picko {
PiCommunicator::PiCommunicator(QString ip, ApiKey key, QObject* _parent)
    : QObject(_parent), printer_(ip, key, this) {}

void PiCommunicator::Connect() {}

void PiCommunicator::Disconnect() { ; }

void PiCommunicator::StartJob(GCode) { ; }
}
