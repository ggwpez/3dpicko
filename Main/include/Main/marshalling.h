#pragma once

#include "Main/global.h"
#include <QDebug>
#include <QJsonObject>
#include <QSet>
#include <opencv2/core.hpp>

#if __cplusplus <= 201703L
// non C++20 code FIXME should be c++20 but is c++17, there is no c++20 define
// yet
/**
 * https://en.cppreference.com/w/cpp/types/type_identity
 */
namespace std
{
template <class T> struct type_identity
{
	using type = T;
};
} // namespace std
#endif

namespace c3picko
{
class Marshalling
{
  public:
	template <typename T> static QJsonObject toJson(T const&);
	template <typename T> static void		 toJson(T const&, QJsonObject& output);
	template <typename T> static T			 fromJson(QJsonObject const&);
	template <typename T> static void		 fromJson(QJsonObject const&, T& output);

	template <typename T> static inline T fromJson(QJsonValue const& val) { return fromJson<T>(val.toObject()); }
};

#define MAKE_SERIALIZABLE(T)                                                                                                              \
	template <> QJsonObject Marshalling::toJson(T const&);                                                                                \
	template <> void		Marshalling::toJson(T const&, QJsonObject&);

#define MAKE_DESEZIALIZABLE(T)                                                                                                            \
	template <> T	Marshalling::fromJson(QJsonObject const&);                                                                           \
	template <> void Marshalling::fromJson(QJsonObject const&, T& output);

#define MAKE_MARSHALLABLE(T)                                                                                                              \
	MAKE_SERIALIZABLE(T)                                                                                                                  \
	MAKE_DESEZIALIZABLE(T)

#define MAKE_MARSHALLABLE_CLASS(T)                                                                                                        \
	class T;                                                                                                                              \
	MAKE_MARSHALLABLE(T)
#define MAKE_MARSHALLABLE_ENUM_CLASS(T)                                                                                                   \
	enum class T;                                                                                                                         \
	MAKE_MARSHALLABLE(T)

// Here we instantiate the template functions, so the compiler knows they must
// exist somewhere. The classes themself are unaltered by this.
MAKE_MARSHALLABLE(QDateTime);
MAKE_MARSHALLABLE(cv::Point2d);
MAKE_MARSHALLABLE(cv::Point2i);
MAKE_MARSHALLABLE(cv::Rect2i);
// template <> inline QJsonObject Marshalling::toJson(std::set<D> const&) {
// return QJsonObject(); }

MAKE_MARSHALLABLE_CLASS(Point);
MAKE_MARSHALLABLE_CLASS(PrinterProfile);
MAKE_MARSHALLABLE_CLASS(PlateSocketProfile);
MAKE_MARSHALLABLE_CLASS(PlateProfile);

// not good style, but partial specialization of template member functions is
// not allowed
MAKE_MARSHALLABLE(QSet<QString>);
MAKE_MARSHALLABLE(QSet<qint32>);

template <>
/**
 * HTML-Escapes all input strings
 */
inline QString Marshalling::fromJson<QString>(QJsonValue const& val)
{
	return val.toString().toHtmlEscaped();
}
} // namespace c3picko
