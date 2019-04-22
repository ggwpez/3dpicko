#ifndef REFERENCE_H
#define REFERENCE_H

#include <QUrl>
class QJsonObject;

namespace c3picko {
namespace pi {
namespace data {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#references)
 */
struct Reference {
  Reference() = default;
  Reference(QJsonObject const &obj);

  QUrl resource, download, model;
};
}  // namespace data
}  // namespace pi
}  // namespace c3picko

#endif  // REFERENCE_H
