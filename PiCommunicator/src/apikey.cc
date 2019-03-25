#include "include/apikey.h"
#include "include/exception.h"

namespace c3picko {
template <>
QJsonObject Marshalling::toJson(const pi::ApiKey& value) {
  QJsonObject obj;

  obj["data"] = value.key();

  return obj;
}

template <>
pi::ApiKey Marshalling::fromJson(const QJsonObject& obj) {
  return pi::ApiKey(obj["data"].toString());
}
namespace pi {
pi::ApiKey::ApiKey(const QString& key) : key_(key) {
// TODO is this official? The docu does not clarify in what format the key is.
// http://docs.octoprint.org/en/master/api/general.html
#ifndef C3PICKO_API_KEY_CHECK_OFF
  if (key.size() != 32)
    throw Exception(
        "Octoprint API key should have length 32 (disable with "
        "-C3PICKO_API_KEY_CHECK_OFF)");

  QRegExp is_hex("^[0-9A-Fa-f]$", Qt::CaseInsensitive);
  if (!is_hex.exactMatch(key))
    throw Exception(
        "Octoprint API key should only contain hexadecimal digits (disable "
        "with -C3PICKO_API_KEY_CHECK_OFF)");
#endif
}

QString ApiKey::key() const { return key_; }
}  // namespace pi
}  // namespace c3picko
