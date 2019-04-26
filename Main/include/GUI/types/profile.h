#pragma once

#include <memory>
#include "GUI/types/profile_type.h"
#include "Main/marshalling.h"

namespace c3picko {
class PrinterProfile;
class PlateSocketProfile;
class PlateProfile;
namespace pi {
class OctoConfig;
}
/**
 * @brief Encapsulator for the 3 GCode profile classes and the OctoPrint
 * profile.
 */
class Profile {
 public:
  typedef QString ID;
  Profile(ProfileType type, QString name, ID id, QJsonObject const& settings);

  explicit operator PrinterProfile*() const;
  explicit operator PlateSocketProfile*() const;
  explicit operator PlateProfile*() const;
  explicit operator pi::OctoConfig*() const;

  ID id() const;
  void setId(ID id);
  QString name() const;
  ProfileType type() const;

  std::shared_ptr<PlateProfile> plate() const;
  std::shared_ptr<PlateSocketProfile> socket() const;
  std::shared_ptr<PrinterProfile> printer() const;
  std::shared_ptr<pi::OctoConfig> octoprint() const;

  /**
   * @brief Creates a correctly formated settings entry for the profile
   * templates.
   * @param min Minimal input length
   * @param max Maximal input length
   */
  static QJsonObject makeTextField(Profile::ID id, QString name,
								   QString description, QString placeholder,
								   QString defaultValue, int min = 0,
								   int max = 512);
  static QJsonObject makeNumberField(Profile::ID id, QString name,
									 QString description, double min,
									 double max, double step,
									 double placeholder, double defaultValue,
									 QString unit);
  /**
   * @brief Creates a field for a 2D point
   * @return
   */
  static QJsonObject makeVec2Field(Profile::ID id, QString name,
								   QString description, Point min, Point max,
								   double step, Point placeholder,
								   Point defaultValue, QString unit);
  /**
   * @brief Creates a field for a 3D point
   */
  static QJsonObject makeVec3Field(Profile::ID id, QString name,
								   QString description, Point min, Point max,
								   double step, Point placeholder,
								   Point defaultValue, QString unit);
  static QJsonObject makeVecField(Profile::ID id, QString name, int vec_size,
								  QString description, Point min, Point max,
								  double step, Point placeholder,
								  Point defaultValue, QString unit);
  /**
   * @brief Creates a radio box
   * @param defaultValue default key used from options
   */
  static QJsonObject makeEnumField(Profile::ID id, QString name,
								   QString description, QVariantMap options,
								   QString defaultValue);

  static QJsonObject printerTemplate();
  static QJsonObject socketTemplate();
  static QJsonObject plateTemplate();
  static QJsonObject octoprintTemplate();

 private:
  ProfileType type_;
  QString name_;
  QString id_;
  std::shared_ptr<PlateProfile> plate_;
  std::shared_ptr<PlateSocketProfile> socket_;
  std::shared_ptr<PrinterProfile> printer_;
  std::shared_ptr<pi::OctoConfig> octoprint_;
};
MAKE_MARSHALLABLE(Profile)
}  // namespace c3picko
