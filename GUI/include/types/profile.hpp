#ifndef PROFILE_HPP
#define PROFILE_HPP

#include <memory>
#include "include/marshalling.hpp"

namespace c3picko {
class PrinterProfile;
class PlateSocketProfile;
class PlateProfile;

/**
 * @brief Encapsulator for the 3 GCode profile classes.
 */
class Profile {
 public:
  typedef QString ID;
  Profile(QString type, QString name, ID id, QJsonObject const& settings);

  explicit operator PrinterProfile*() const;
  explicit operator PlateSocketProfile*() const;
  explicit operator PlateProfile*() const;

  ID id() const;
  void setId(ID id);
  QString name() const;
  QString type() const;

  std::shared_ptr<PlateProfile> plate() const;
  std::shared_ptr<PlateSocketProfile> socket() const;
  std::shared_ptr<PrinterProfile> printer() const;

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

 private:
  QString type_;
  QString name_;
  QString id_;
  std::shared_ptr<PlateProfile> plate_;
  std::shared_ptr<PlateSocketProfile> socket_;
  std::shared_ptr<PrinterProfile> printer_;
};
MAKE_MARSHALLABLE(Profile);
}  // namespace c3picko

#endif  // PROFILE_HPP
