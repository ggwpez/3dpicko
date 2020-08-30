/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#pragma once

#include <QDateTime>
#include <QImage>
#include <opencv2/opencv.hpp>

#include "Main/global.h"
#include "Main/resource_path.h"

namespace c3picko {
/**
 * @brief Represents a cached image. Internal format is cv::Mat.
 */
class Image {
 public:
  typedef QString ID;
  Image() = default;
  Image(ID id, QString original_name, QString description, ResourcePath path,
		QDateTime uploaded, int width, int hwight);
  Image(QByteArray data, QString original_name, QString description,
		QDateTime uploaded = QDateTime::currentDateTime());
  Image(cv::Mat image, QString original_name, QString description,
		QDateTime uploaded = QDateTime::currentDateTime());

  /**
   * @brief Writes the image into its path_ file.
   * Sets path_ to the path of the new file.
   * Clears out image_.
   * @return Wheter it was successful.
   */
  bool writeToFile();
  /**
   * @brief Delete the file from the hard drive.
   * Does not clear Cached data.
   * @return
   */
  bool deleteFile();
  /**
   * @brief Clears all cached image data.
   */
  void clearCache();

  /**
   * @brief Calculates a unique id (hash) for the given matrix
   * @return
   */
  static ID calculateId(const cv::Mat& image);

  bool readCvMat(cv::Mat& output);
  bool readData(QByteArray& output) const;
  static bool decodeCvMat(QByteArray data, cv::Mat& output);

  ResourcePath path() const;
  ID id() const;
  QString originalName() const;
  int width() const;
  int height() const;
  QString description() const;
  QDateTime uploaded() const;

 private:
  QString original_name_;
  QString description_;
  // TODO we can add meta data here
  /**
   * @brief The relative file path, not starting with /
   */
  ResourcePath path_;
  QDateTime uploaded_;
  /**
   * @brief The image itself. Also referred to as the 'cache'.
   */
  cv::Mat image_;
  int width_ = 0, height_ = 0;
  ID id_;
};
MAKE_MARSHALLABLE(Image);
}  // namespace c3picko
