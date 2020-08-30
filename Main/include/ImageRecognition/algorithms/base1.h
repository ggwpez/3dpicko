/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <opencv2/core.hpp>

#include "ImageRecognition/algorithm.h"
#include "ImageRecognition/colony.h"
#include "ImageRecognition/detection_result.h"

namespace c3picko {
/**
 * @brief Image detection algorithm for normal light, more description in the
 * wiki.
 */
class Normal1 : public Algorithm {
  Q_OBJECT
 public:
  Normal1();
  virtual ~Normal1() override = default;

  static void cvt(AlgorithmJob* base, DetectionResult* result);
  static void mask(AlgorithmJob* base, DetectionResult* result);
  static void threshold(AlgorithmJob* base, DetectionResult* result);
  static void erodeAndDilate(AlgorithmJob* base, DetectionResult* result);
  static void label(AlgorithmJob* base, DetectionResult* result);
  static void relativeFiltering(AlgorithmJob* base, DetectionResult* result);

  virtual inline Algorithm* cloneEmpty() const override {
	return new Normal1();
  }

  static void safetyMargin(AlgorithmJob* base, DetectionResult* result);

 private:
};
}  // namespace c3picko
