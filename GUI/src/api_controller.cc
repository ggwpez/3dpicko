#include "include/api_controller.h"
#include "include/api_input.h"
#include "include/api_output.h"
#include <QMetaMethod>

namespace c3picko {

	APIController::APIController(QThreadPool* pool, Database& db, QObject* parent)
		: QObject (parent), pool_(pool), db_(db),
		  input_(new APIInput(this)), output_(new APIOutput(this))
	{
		QMetaObject const* meta = this->metaObject();

		for (int i = meta->methodOffset(); i < meta->methodCount(); ++i)
		{
			QMetaMethod signal = meta->method(i);
			QString slot_name = QMetaObject::normalizedSignature(qPrintable(signal.methodSignature()));

			if (signal.methodType() == QMetaMethod::Signal)
			{
				if (! slot_name.startsWith("On"))
					continue;
				slot_name = slot_name.mid(2);

				int slot_index = output_->metaObject()->indexOfSlot(qPrintable(slot_name));

				if (slot_index == -1)
					qDebug("Could not find slot APIOutput::%s", qPrintable(slot_name));
				{
					QMetaMethod slot = output_->metaObject()->method(slot_index);

					QObject::connect(this, signal, output_, slot);
				}
			}
		}
	}

	void APIController::service(QJsonObject& request, QObject* client)
	{
		input_->service(request, client);
	}

	Database& APIController::db() const
	{
		return db_;
	}

	QThreadPool*APIController::pool() const
	{
		return pool_;
	}
}
