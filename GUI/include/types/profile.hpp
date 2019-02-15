#ifndef PROFILE_HPP
#define PROFILE_HPP

#include "include/json_constructable.hpp"
#include <memory>

namespace c3picko {
class PrinterProfile;
class PlateSocketProfile;
class PlateProfile;

class Profile : public JsonConstructable {
public:
  typedef QString ID;
  Profile(QJsonObject const &);

  void write(QJsonObject &obj) const;

  explicit operator PrinterProfile *() const;
  explicit operator PlateSocketProfile *() const;
  explicit operator PlateProfile *() const;

  ID id() const;
  void setId(ID id);
  QString name() const;

private:
  QString type_;
  QString name_;
  QString id_;
  std::shared_ptr<PlateProfile> plate_;
  std::shared_ptr<PlateSocketProfile> socket_;
  std::shared_ptr<PrinterProfile> printer_;
};
} // namespace c3picko

#endif // PROFILE_HPP
