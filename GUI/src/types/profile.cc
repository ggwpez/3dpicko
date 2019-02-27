#include "include/types/profile.hpp"
#include <QDebug>
#include "include/marshalling.hpp"
#include "include/plateprofile.h"
#include "include/platesocketprofile.h"
#include "include/printerprofile.h"

namespace c3picko {
Profile::Profile(QString type, QString name, ID id, QJsonObject const &settings)
    : type_(type), name_(name), id_(id) {
  if (type_ == "printer-profile") {
    printer_ = std::make_shared<PrinterProfile>(
        Marshalling::fromJson<PrinterProfile>(settings));
  } else if (type_ == "socket-profile") {
    socket_ = std::make_shared<PlateSocketProfile>(
        Marshalling::fromJson<PlateSocketProfile>(settings));
  } else if (type_ == "plate-profile") {
    plate_ = std::make_shared<PlateProfile>(
        Marshalling::fromJson<PlateProfile>(settings));
  } else {
    plate_ = nullptr;
    qWarning() << "Unknown profile type" << type_;
  }
}

Profile::ID Profile::id() const { return id_; }

void Profile::setId(Profile::ID id) { id_ = id; }

QString Profile::name() const { return name_; }

QString Profile::type() const { return type_; }

std::shared_ptr<PlateProfile> Profile::plate() const { return plate_; }

std::shared_ptr<PlateSocketProfile> Profile::socket() const { return socket_; }

std::shared_ptr<PrinterProfile> Profile::printer() const { return printer_; }

c3picko::Profile::operator PlateProfile *() const { return plate_.get(); }

c3picko::Profile::operator PlateSocketProfile *() const {
  return socket_.get();
}

c3picko::Profile::operator PrinterProfile *() const { return printer_.get(); }

template <>
QJsonObject Marshalling::toJson(const Profile &value) {
  QJsonObject obj;

  // TODO also dumb
  QJsonObject settings;
  if (value.type() == "printer-profile")
    settings = Marshalling::toJson(*value.printer());
  else if (value.type() == "socket-profile")
    settings = Marshalling::toJson(*value.socket());
  else if (value.type() == "plate-profile")
    settings = Marshalling::toJson(*value.plate());
  else
    qWarning() << "Cant write profile of unknown type";

  obj["settings"] = settings;
  obj["type"] = value.type();
  obj["profile_name"] = value.name();
  obj["id"] = value.id();

  return obj;
}

template <>
Profile Marshalling::fromJson(const QJsonObject &obj) {
  return Profile(obj["type"].toString(), obj["profile_name"].toString(),
                 obj["id"].toString(), obj["settings"].toObject());
}
}  // namespace c3picko
