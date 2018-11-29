#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <QJsonObject>

namespace c3picko {
namespace commands {
namespace responses {
struct Response {
  Response() = default;
  inline Response(QJsonObject) {}
  virtual inline ~Response() {}
};
}
}
}

#endif  // RESPONSE_H_
