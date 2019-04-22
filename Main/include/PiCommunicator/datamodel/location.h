#ifndef ORIGIN_H
#define ORIGIN_H

class QString;

namespace c3picko {
namespace pi {
namespace data {
///
/// [API](http://docs.octoprint.org/en/master/api/datamodel.html#files)
///
enum class Location : char { LOCAL, SD_CARD };

Location LocationFromString(QString const &str);
QString ToString(Location val);
}  // namespace data
}  // namespace pi
}  // namespace c3picko

#endif  // ORIGIN_H
