#include "datamodel/location.h"
#include <QString>
#include "include/exception.h"

namespace c3picko {
namespace pi {
namespace data {
Location LocationFromString(const QString &str) {
  if (str == "local") return Location::LOCAL;
  if (str == "sdcard")
    return Location::SD_CARD;
  else
    throw Exception("Origin type unknown");
}

QString ToString(Location val) {
  if (val == Location::LOCAL)
    return "local";
  else
    return "sdcard";
}

}  // namespace data
}  // namespace pi
}  // namespace c3picko
