#include "datamodel/reference.h"
#include <QJsonObject>

namespace c3picko {
namespace pi {
namespace data {
Reference::Reference(const QJsonObject &obj) {
  resource = obj["resource"].toString();

  if (obj.contains("download")) download = obj["download"].toString();
  if (obj.contains("model")) download = obj["model"].toString();
}
}  // namespace data
}  // namespace pi
}  // namespace c3picko
