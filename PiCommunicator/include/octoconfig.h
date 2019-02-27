#ifndef OCTO_PROFILE_H
#define OCTO_PROFILE_H

#include "include/apikey.h"
#include "include/marshalling.hpp"

namespace c3picko {
namespace pi {
/**
 * @brief Saves the configuration for class OctoPrint
 */
class OctoConfig {
 public:
  /**
   * @brief OctoProfile
   * @param address Domain or IP address of octoprint.
   * @param protocol Default is http. Can be set to https, if octoprint
   * will _eventually_ support https.
   */
  OctoConfig(QString address, ApiKey const &key, QString protocol = "http",
             quint16 port = 80);

  QString address() const;
  ApiKey key() const;
  QString protocol() const;
  quint16 port() const;

 private:
  /**
   * @brief Unresolved doman or ip.
   */
  QString address_;
  ApiKey key_;
  QString protocol_;
  quint16 port_;
};
}  // namespace pi
MAKE_MARSHALLABLE(pi::OctoConfig);
}  // namespace c3picko

#endif  // OCTO_PROFILE_H