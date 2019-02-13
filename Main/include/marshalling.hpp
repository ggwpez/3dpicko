#pragma once

#include <QJsonObject>

namespace c3picko {
class Marshalling {
public:
  template <typename T> static QJsonObject toJson(T const &);

  template <typename T> static void toJson(T const &, QJsonObject &output);

  template <typename T> static T fromJson(QJsonObject const &);

  template <typename T> static void fromJson(QJsonObject const &, T &output);
};

#define MAKE_MARSHALLABLE(T)                                                   \
                                                                               \
  template <> QJsonObject Marshalling::toJson(T const &);                      \
  template <> void Marshalling::toJson(T const &, QJsonObject &);              \
  template <> T Marshalling::fromJson(QJsonObject const &);                    \
  template <> void Marshalling::fromJson(QJsonObject const &, T &output);

#define MAKE_MARSHALLABLE_CLASS(T)                                             \
  class T;                                                                     \
  MAKE_MARSHALLABLE(T)
#define MAKE_MARSHALLABLE_ENUM_CLASS(T)                                        \
  enum class T;                                                                \
  MAKE_MARSHALLABLE(T)

// Here we instantiate the template functions, so the compiler knows they must
// exist somewhere. The classes themself are unaltered by this.
MAKE_MARSHALLABLE_CLASS(Point);
MAKE_MARSHALLABLE_CLASS(PrinterProfile);
MAKE_MARSHALLABLE_CLASS(PlateSocketProfile);
MAKE_MARSHALLABLE_CLASS(PlateProfile);
} // namespace c3picko
