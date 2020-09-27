/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QString>
#include <vector>

#include "ImageRecognition/colony.h"

namespace cv {
class Mat;
}
namespace d3picko {
class Colony;
/**
 * @brief Represents a failed or succeeded AlgorithmJob.
 */
class AlgorithmResult {
 public:
  typedef QString ID;

  AlgorithmResult(ID id);
  virtual ~AlgorithmResult();

  /**
   * @brief Will be called after the last step of the algorithm succeeded
   */
  virtual void finalize();

 public:
  ID id_;
  QDateTime created_;
  /**
   * @brief Explicit cleanup for debugging
   */

  /**
   * @brief The job succeeded
   */
  bool stages_succeeded_;
  /**
   * @brief The cleanup stage succeeded
   */
  bool cleanup_succeeded_;

  /**
   * @brief Last stage that the job was in.
   */
  int last_stage_;
  QString stage_error_, cleanup_error_;
  bool is_finalized_ = false;

  ID id() const;
  bool stagesSucceeded() const;
  bool cleanupSucceeded() const;
  int lastStage() const;
  quint64 tookNs() const;
  QString cleanupError() const;
  QString stageError() const;
  QDateTime created() const;
};
MAKE_SERIALIZABLE(AlgorithmResult);
}  // namespace d3picko
