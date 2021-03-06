/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include "Main/marshalling.h"

namespace d3picko {
/**
 * @brief Represents a file path.
 */
class ResourcePath {
  ResourcePath(QString system_absolute_path);

 public:
  /**
   * @brief Construct empty
   */
  ResourcePath();
  /**
   * @brief Absolute path
   * ex: /home/user/Code/3dpicko/GUI/uploads/test.jpg
   */
  static ResourcePath fromSystemAbsolute(QString path);
  /**
   * @brief Absolute path, relative to execution directory
   * ex: ../../3dpicko/GUI/uploads/test.jpg
   */
  static ResourcePath fromSystemRelative(QString path);

  /**
   * @brief Path absolute to Root()
   * ex: /uploads/test.jpg
   */
  static ResourcePath fromServerAbsolute(QString path);
  /**
   * @brief Path relative to Root()
   * ex: uploads/test.jpg
   */
  static ResourcePath fromServerRelative(QString path);
  static ResourcePath fromDocRootAbsolute(QString path);

  const QString toSystemAbsolute() const;
  const QString toServerAbsolute() const;
  /**
   * @brief ex: /uploads/123.jpg
   */
  const QString toDocRootAbsolute() const;

  /**
   * @brief Method will throw, when path does not exist
   */
  bool exists() const;
  /**
   * @brief Method will throw, when the path does not exist or is not a
   * direcrory
   */
  bool isDir() const;
  bool isFile() const;

  bool isEmpty() const;
  void clear();
  ResourcePath operator+(QString const&) const;

 private:
  QString system_absolute_;
};
MAKE_MARSHALLABLE(ResourcePath);
}  // namespace d3picko
