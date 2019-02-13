#pragma once

#include "include/colony.hpp"

namespace c3picko {
enum class Colony::Type {
  INCLUDED,
  EXCLUDED_BY_USER, // User manually excluded this one

  EXCLUDED_BY_AABB_RATIO,
  EXCLUDED_BY_BB_RATIO,
  EXCLUDED_BY_CIRCULARITY,
  EXCLUDED_BY_CONVEXITY,

  // Not in yet
  EXCLUDED_BY_BRIGHTNESS,
  EXCLUDED_BY_AREA
};

MAKE_MARSHALLABLE(Colony::Type);
} // namespace c3picko
