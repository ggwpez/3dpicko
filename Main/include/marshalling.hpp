#pragma once

#include <QJsonObject>

namespace c3picko {
class Marshalling {
 public:
  template <typename T>
  static QJsonObject toJson(T const&);

  template <typename T>
  static void toJson(T const&, QJsonObject& output);

  template <typename T>
  static T fromJson(QJsonObject const&);

  template <typename T>
  static void fromJson(QJsonObject const&, T& output);
};

#define MAKE_MARSHALLABLE(T)                        \
													\
  class T;                                          \
  template <>                                       \
  QJsonObject Marshalling::toJson(T const&);        \
  template <>                                       \
  void Marshalling::toJson(T const&, QJsonObject&); \
  template <>                                       \
  T Marshalling::fromJson(QJsonObject const&);      \
  template <>                                       \
  void Marshalling::fromJson(QJsonObject const&, T& output);

// Here we instantiate the template functions, so the compiler knows they must
// exist somewhere. The classes themself are unaltered by this.
MAKE_MARSHALLABLE(Point);
MAKE_MARSHALLABLE(PrinterProfile);
MAKE_MARSHALLABLE(PlateSocketProfile);
MAKE_MARSHALLABLE(PlateProfile);
}
