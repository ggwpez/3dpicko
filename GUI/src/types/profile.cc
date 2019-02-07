#include "include/types/profile.hpp"
#include "include/plateprofile.h"
#include "include/platesocketprofile.h"
#include "include/printerprofile.h"
#include <QDebug>

namespace c3picko
{
	ProfileWrapper::ProfileWrapper(const QJsonObject& obj)
		: JsonConstructable(obj), type_(obj["type"].toString()), name_(obj["profile_name"].toString()), id_(obj["id"].toString())
	{
		if (type_ == "printer-profile")
			printer_ = std::make_shared<PrinterProfile>(obj);
		else if (type_ == "socket-profile")
			socket_ = std::make_shared<PlateSocketProfile>(obj);
		else if (type_ == "plate-profile")
			plate_ = std::make_shared<PlateProfile>(obj);
		else
		{
			plate_ = nullptr;
			qWarning() << "Unknown profile type" << type_;
		}
	}

	void ProfileWrapper::write(QJsonObject& obj) const
	{
		obj["type"] = type_;
		obj["profile_name"] = name_;
		obj["id"] = id_;

		// TODO also dumb
		if (type_ == "printer-profile")
			printer_->write(obj);
		else if (type_ == "socket-profile")
			socket_->write(obj);
		else if (type_ == "plate-profile")
			plate_->write(obj);
		else
			qWarning() << "Cant write profile of unknown type";
	}

	ProfileWrapper::ID ProfileWrapper::id() const
	{
		return id_;
	}

	QString ProfileWrapper::name() const
	{
		return name_;
	}

	c3picko::ProfileWrapper::operator PlateProfile*() const
	{
		return plate_.get();
	}

	c3picko::ProfileWrapper::operator PlateSocketProfile*() const
	{
		return socket_.get();
	}

	c3picko::ProfileWrapper::operator PrinterProfile*() const
	{
		return printer_.get();
	}
}
