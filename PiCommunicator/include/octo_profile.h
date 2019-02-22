#ifndef OCTO_PROFILE_H
#define OCTO_PROFILE_H

#include "include/apikey.h"
#include "include/marshalling.hpp"

namespace c3picko {
namespace pi {
/**
 * @brief Saves the configuration for class OctoPrint
 */
class OctoProfile {
 public:
  OctoProfile(QString address, quint16 port, ApiKey const& key);

  QString address() const;
  quint16 port() const;
  ApiKey key() const;

 private:
  /**
   * @brief Unresolved doman or ip.
   */
  QString address_;
  quint16 port_;
  ApiKey key_;
};
}  // namespace pi
MAKE_MARSHALLABLE(pi::OctoProfile);
}  // namespace c3picko

#endif  // OCTO_PROFILE_H
