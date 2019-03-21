#ifndef APIKEY_H_
#define APIKEY_H_

#include "include/marshalling.hpp"
#include <QString>

namespace c3picko {
namespace pi {
typedef QString ApiKey;
}
MAKE_MARSHALLABLE(pi::ApiKey);
} // namespace c3picko

#endif // APIKEY_H_
