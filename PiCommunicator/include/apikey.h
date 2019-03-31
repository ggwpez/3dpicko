#ifndef APIKEY_H_
#define APIKEY_H_

#include <QString>
#include "include/marshalling.hpp"

namespace c3picko {
namespace pi {
struct ApiKey {
  ApiKey(QString const& key);

  QString key() const;

 private:
  QString key_;
};
}  // namespace pi
MAKE_MARSHALLABLE(pi::ApiKey);
}  // namespace c3picko

#endif  // APIKEY_H_
