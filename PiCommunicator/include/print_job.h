#ifndef PRINT_JOB_H
#define PRINT_JOB_H

#include "include/gcode.h"
#include <QObject>

namespace c3picko
{
namespace pi
{
	class PrintJob : public QObject
	{
		Q_OBJECT

	  signals:
		void OnError(QString);
		/**
		 * @brief Job was successfully send to the printer,
		 * either as UploadFile or ArbitratyCommand
		 */
		void OnUploaded();
		/**
		 * @brief Job started printing
		 */
		void OnStarted();
		/**
		 * @brief Job finished printing.
		 *
		 * Use for cleanup.
		 */
		void OnFinished();
	};
}
}

#endif // PRINT_JOB_H
