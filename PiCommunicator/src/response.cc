#include "response.h"
#include <QJsonDocument>

namespace c3picko {
namespace pi {
namespace responses {
Response::Response(const QJsonObject &obj) : raw(obj) {}

QString Response::ToString() {
  QJsonDocument doc(raw);
  return doc.toJson();
}
}  // namespace responses
}  // namespace pi
}  // namespace c3picko
