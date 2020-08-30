/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include "ImageRecognition/algorithm.h"
#include "ImageRecognition/detection_result.h"

namespace c3picko {
namespace algorithms {
class Fluro1 : public Algorithm {
  Q_OBJECT
 public:
  Fluro1();

  static void threshold(AlgorithmJob* base, DetectionResult* result);

  virtual inline Algorithm* cloneEmpty() const override { return new Fluro1(); }
};
}  // namespace algorithms
}  // namespace c3picko
