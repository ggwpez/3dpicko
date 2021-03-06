/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include "ImageRecognition/algo_setting.h"
#include "Main/global.h"
#include "Main/marshalling.h"

namespace d3picko {

class Colony {
 public:
  typedef qint32 ID;

  Colony(double x, double y, double area, double circumference,
		 double major_length, double brightness, ID id,
		 AlgoSetting::ID excluded_by);

  /**
   * @brief x position relative to image [0,1]
   */
  double x() const;
  /**
   * @brief y position relative to image [0,1]
   */
  double y() const;
  double area() const;
  double circumference() const;
  double major_length() const;
  double brightness() const;
  ID id() const;

  bool included() const;
  AlgoSetting::ID excludedBy() const;

  void setExcludedBy(const AlgoSetting::ID& excluded_by);

 private:
  /**
   * @brief Center X position relative to the image size. Value in interval
   * [0,1]
   */
  double x_;
  /**
   * @brief Center Y position relative to the image size. Value in interval
   * [0,1]
   */
  double y_;

  /**
   * @brief Estimated area in pixel
   */
  double area_;

  /**
   * @brief Estimated circumference in pixel widths
   */
  double circumference_;

  /**
   * @brief Length of the major axis when laying a minimum area ellipse over the
   * colony relative to the image size. Value in interval [0,1].
   *
   * Use this as size when drawing bounding cirles/rectangles!
   */
  double major_length_;

  /**
   * @brief Absolute detected brightness of the colony. Value in intervall [0,1]
   */
  double brightness_;

  ID id_;

  /**
   * @brief Indicates whether a colony was excluded or not.
   * Is empty if the colony was not excluded and otherwise set to the setting
   * preventing the inclusion.
   */
  AlgoSetting::ID excluded_by_;
};

MAKE_MARSHALLABLE(Colony);
}  // namespace d3picko
