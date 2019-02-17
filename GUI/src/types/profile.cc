#include "include/types/profile.hpp"
#include "include/marshalling.hpp"
#include "include/plateprofile.h"
#include "include/platesocketprofile.h"
#include "include/printerprofile.h"
#include <QDebug>

namespace c3picko
{
Profile::Profile(const QJsonObject& obj)
	: JsonConstructable(obj), type_(obj["type"].toString()), name_(obj["profile_name"].toString()), id_(obj["id"].toString())
{

	QJsonObject settings = obj["settings"].toObject();

	if (type_ == "printer-profile")
	{
		printer_ = std::make_shared<PrinterProfile>(Marshalling::fromJson<PrinterProfile>(settings));
	}
	else if (type_ == "socket-profile")
	{
		socket_ = std::make_shared<PlateSocketProfile>(Marshalling::fromJson<PlateSocketProfile>(settings));
	}
	else if (type_ == "plate-profile")
	{
		plate_ = std::make_shared<PlateProfile>(Marshalling::fromJson<PlateProfile>(settings));
	}
	else
	{
		plate_ = nullptr;
		qWarning() << "Unknown profile type" << type_;
	}
}

void Profile::write(QJsonObject& obj) const
{
	// TODO also dumb
	QJsonObject settings;
	if (type_ == "printer-profile")
		settings = Marshalling::toJson(*printer_);
	else if (type_ == "socket-profile")
		settings = Marshalling::toJson(*socket_);
	else if (type_ == "plate-profile")
		settings = Marshalling::toJson(*plate_);
	else
		qWarning() << "Cant write profile of unknown type";

	obj["settings"]		= settings;
	obj["type"]			= type_;
	obj["profile_name"] = name_;
	obj["id"]			= id_;
}

Profile::ID Profile::id() const { return id_; }

void Profile::setId(Profile::ID id) { id_ = id; }

QString Profile::name() const { return name_; }

c3picko::Profile::operator PlateProfile*() const { return plate_.get(); }

c3picko::Profile::operator PlateSocketProfile*() const { return socket_.get(); }

c3picko::Profile::operator PrinterProfile*() const { return printer_.get(); }
} // namespace c3picko
