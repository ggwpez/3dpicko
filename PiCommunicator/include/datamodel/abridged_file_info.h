#ifndef ABRIDGED_FILE_INFORMATION_H_
#define ABRIDGED_FILE_INFORMATION_H_

#include "datamodel/location.h"
#include "datamodel/reference.h"
#include "response.h"
#include <QString>

class QJsonObject;

namespace c3picko {
namespace pi {
namespace data {
/**
 * @brief
 * [API](http://docs.octoprint.org/en/master/api/datamodel.html#sec-api-datamodel-files-file)
 *
 * Sadly this is a datamodel and a response, so it needs to be both…
 */
struct AbridgedFileInfo : public responses::Response {
  AbridgedFileInfo() = default;
  AbridgedFileInfo(QJsonObject const &obj);

  QString name, display, path;
  data::Location origin;
  data::Reference ref;
};
} // namespace data
namespace responses {
typedef data::AbridgedFileInfo AbridgedFileInfo;
}
} // namespace pi
} // namespace c3picko

#endif // ABRIDGED_FILE_INFORMATION_H_
