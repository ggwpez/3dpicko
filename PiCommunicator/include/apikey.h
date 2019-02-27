#ifndef APIKEY_H_
#define APIKEY_H_

#include <QString>
#include "include/marshalling.hpp"

namespace c3picko {
namespace pi {
typedef QString ApiKey;
}
MAKE_MARSHALLABLE(pi::ApiKey);
}  // namespace c3picko

#endif  // APIKEY_H_
