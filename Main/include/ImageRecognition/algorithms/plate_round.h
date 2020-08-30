/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include "ImageRecognition/algorithm.h"
#include "ImageRecognition/colony.h"

namespace d3picko {
class PlateResult;
namespace algorithms {
/**
 * @brief Algorithm for detecting a round Plate on an image under normal
 * light.
 *
 * TODO link
 */
class PlateRound : public Algorithm {
  Q_OBJECT
 public:
  PlateRound();

  static void detect(AlgorithmJob* base, PlateResult* result);

  virtual inline Algorithm* cloneEmpty() const override {
	return new PlateRound();
  }
};
}  // namespace algorithms
}  // namespace d3picko
