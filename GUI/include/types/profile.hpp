#ifndef PROFILE_HPP
#define PROFILE_HPP

#include "include/marshalling.hpp"
#include <memory>

namespace c3picko
{
class PrinterProfile;
class PlateSocketProfile;
class PlateProfile;

class Profile
{
  public:
	typedef QString ID;
	Profile(QString type, QString name, ID id, QJsonObject const& settings);

	void write(QJsonObject& obj) const;

	explicit operator PrinterProfile*() const;
	explicit operator PlateSocketProfile*() const;
	explicit operator PlateProfile*() const;

	ID   id() const;
	void setId(ID id);
	QString name() const;
	QString type() const;

	std::shared_ptr<PlateProfile>		plate() const;
	std::shared_ptr<PlateSocketProfile> socket() const;
	std::shared_ptr<PrinterProfile>		printer() const;

  private:
	QString								type_;
	QString								name_;
	QString								id_;
	std::shared_ptr<PlateProfile>		plate_;
	std::shared_ptr<PlateSocketProfile> socket_;
	std::shared_ptr<PrinterProfile>		printer_;
};
MAKE_MARSHALLABLE(Profile);
} // namespace c3picko

#endif // PROFILE_HPP
