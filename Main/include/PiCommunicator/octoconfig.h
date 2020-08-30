/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef OCTO_PROFILE_H
#define OCTO_PROFILE_H

#include "Main/marshalling.h"
#include "PiCommunicator/apikey.h"

namespace d3picko {
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
  OctoConfig(QString address, ApiKey const& key, QString protocol = "http",
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
}  // namespace d3picko

#endif	// OCTO_PROFILE_H
