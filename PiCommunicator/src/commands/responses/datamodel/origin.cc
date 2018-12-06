#include "commands/responses/datamodel/origin.h"

namespace c3picko {
namespace commands {
namespace responses {
Origin OriginFromString(QString str) {
  if (str == "local")
    return Origin::LOCAL;
  if (str == "sdcard")
    return Origin::SD_CARD;
  else
    throw std::runtime_error("Origin type unknown");
}
} // namespace responses
} // namespace commands
} // namespace c3picko
