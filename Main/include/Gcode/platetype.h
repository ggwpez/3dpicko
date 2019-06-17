#pragma once

#include "Main/marshalling.h"

namespace c3picko {
/**
 * @brief Type of the plate, currently plates can be round or rectangular
 */
enum class PlateType { kROUND, kRECT };

QString toString(PlateType);
PlateType plateTypeFromString(QString const&);

MAKE_MARSHALLABLE(PlateType);
}  // namespace c3picko
