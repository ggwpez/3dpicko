/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QString>

namespace d3picko {
/**
 * @brief List of all permitted API commands to eliminate spelling mistakes.
 */
struct APICommands {
  static QString GET_IMAGE_LIST;
  static QString GET_PROFILE_LIST;
  static QString GET_VERSION_LIST;
  static QString GET_JOB_LIST;
  static QString VERSION_SWITCHED;
  static QString GET_DETECTION_ALGORITHMS;
  static QString SET_DEFAULT_SETTINGS_PROFILE;
  static QString SET_STARTING_WELL;
  static QString DELETE_IMAGE;
  static QString DELETE_JOB;
  static QString START_JOB;
  static QString GET_REPORT;
  static QString UPLOAD_IMAGE;
  static QString UPDATE_SOFTWARE;
  static QString CREATE_SETTINGS_PROFILE;
  static QString UPDATE_SETTINGS_PROFILE;
  static QString DELETE_SETTINGS_PROFILE;
  static QString CREATE_JOB;
  static QString GET_POSITIONS;
  static QString SET_COLONIES_TO_PICK;
  static QString UPDATE_DETECTION_SETTINGS;
  static QString SHUTDOWN;
  static QString RESTART;
  static QString BACKUP;
};
}  // namespace d3picko
