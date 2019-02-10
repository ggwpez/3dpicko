#ifndef PROFILE_HPP
#define PROFILE_HPP

#include <memory>
#include "include/json_constructable.hpp"

namespace c3picko {
class PrinterProfile;
class PlateSocketProfile;
class PlateProfile;

class Profile : public JsonConstructable {
 public:
  typedef QString ID;
  Profile(QJsonObject const&);

  void write(QJsonObject& obj) const;

  explicit operator PrinterProfile*() const;
  explicit operator PlateSocketProfile*() const;
  explicit operator PlateProfile*() const;

  ID id() const;
  QString name() const;

 private:
  QString type_;
  QString name_;
  QString id_;
  std::shared_ptr<PlateProfile> plate_;
  std::shared_ptr<PlateSocketProfile> socket_;
  std::shared_ptr<PrinterProfile> printer_;
};
}

#endif  // PROFILE_HPP
