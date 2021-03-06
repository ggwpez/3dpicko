/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "GUI/types/image.h"

#include <QBuffer>
#include <QCryptographicHash>
#include <QDebug>
#include <QTemporaryFile>
#include <algorithm>

#include "Main/resource_path.h"

namespace d3picko {
Image::Image(Image::ID id, QString original_name, QString description,
			 ResourcePath path, QDateTime uploaded, int width, int height)
	: original_name_(original_name),
	  description_(description),
	  path_(path),
	  uploaded_(uploaded),
	  width_(width),
	  height_(height),
	  id_(id) {
  Q_ASSERT(!id_.isEmpty());
  if (path_.isEmpty()) qWarning() << "Image path was empty, image lost";
  Q_ASSERT(width_ && height_);
}

Image::Image(QByteArray data, QString original_name, QString description,
			 QDateTime uploaded)
	: original_name_(original_name),
	  description_(description),
	  path_(),
	  uploaded_(uploaded) {
  if (!decodeCvMat(data, image_)) {
	qWarning() << "Loading image failed:" << original_name;
	return;
  }
  width_ = image_.cols;
  height_ = image_.rows;
  id_ = calculateId(image_);

  Q_ASSERT(!id_.isEmpty());
  Q_ASSERT(width_ && height_);
}

Image::Image(cv::Mat image, QString original_name, QString description,
			 QDateTime uploaded)
	: original_name_(original_name),
	  description_(description),
	  uploaded_(uploaded),
	  image_(image),
	  width_(image.cols),
	  height_(image.rows),
	  id_(calculateId(image_)) {
  Q_ASSERT(!id_.isEmpty());
  Q_ASSERT(path_.isEmpty());
  Q_ASSERT(width_ && height_);
}

bool Image::writeToFile() {
  if (image_.empty() ||
	  !path_.isEmpty())	 // path_ should be empty, otherwise we may create two
						 // files with the same content
	return false;

  QTemporaryFile file((paths::uploadFolder() + "XXXXXXXX").toSystemAbsolute());
  file.setAutoRemove(false);  // TODO

  if (!file.open()) {
	qCritical() << "Could not write image to drive:" << file.errorString()
				<< "Directory:" << paths::uploadFolder().toSystemAbsolute();
	return false;
  } else {
	std::vector<uint8_t> raw;
	cv::imencode(defaultImageExtension(), image_, raw);

	if (!file.write(reinterpret_cast<char const*>(raw.data()), raw.size())) {
	  qCritical() << "Could not write image" << id_ << "(" << file.errorString()
				  << ")";
	  return false;
	}

	path_ = paths::uploadFolder() + QFileInfo(file).fileName();
	return true;
  }
}

bool Image::deleteFile() {
  if (QDir().remove(path_.toSystemAbsolute())) {
	path_.clear();
	return true;
  }
  return false;
}

void Image::clearCache() { image_.release(); }

bool Image::readCvMat(cv::Mat& output) {
  if (!(output = image_).empty()) return true;

  QByteArray data;
  if (!readData(data)) return false;

  if (!decodeCvMat(data, image_))
	return false;
  else {
	output = image_;
	return true;
  }
}

bool Image::readData(QByteArray& output) const {
  output = {};
  QFile file(path_.toSystemAbsolute());

  if (!file.open(QIODevice::ReadOnly)) {
	qWarning() << "Could not open file:" << path_.toSystemAbsolute() << ":"
			   << file.errorString();
	return false;
  }

  output = file.readAll();
  return true;
}

bool Image::decodeCvMat(QByteArray data, cv::Mat& output) {
  if (data.isEmpty()) return false;

  output = cv::imdecode(cv::Mat(1, data.size(), CV_8UC1, data.data()),
						cv::IMREAD_COLOR);
  return (!output.empty());
}

QDateTime Image::uploaded() const { return uploaded_; }

QString Image::description() const { return description_; }

Image::ID Image::calculateId(cv::Mat const& image) {
  QCryptographicHash hasher(QCryptographicHash::Sha256);

  std::vector<char> data;  // TODO this is not gud
  for (int i = 0; i < image.rows; ++i)
	data.insert(data.end(), image.ptr<uint8_t>(i),
				image.ptr<uint8_t>(i) + image.cols);

  hasher.addData(data.data(), data.size());
  return hasher.result().toHex();
}

Image::ID Image::id() const { return id_; }

int Image::width() const { return width_; }

int Image::height() const { return height_; }

QString Image::originalName() const { return original_name_; }

ResourcePath Image::path() const { return path_; }

template <>
QJsonObject Marshalling::toJson(const Image& value) {
  QJsonObject obj;

  obj["id"] = value.id();
  obj["original_name"] = value.originalName();
  obj["description"] = value.description();
  obj["path"] = value.path().toDocRootAbsolute();
  obj["uploaded"] = Marshalling::toJson(value.uploaded());
  obj["width"] = value.width();
  obj["height"] = value.height();

  return obj;
}

template <>
Image Marshalling::fromJson(const QJsonObject& obj) {
  return Image(Marshalling::fromJson<QString>(obj["id"]),
			   Marshalling::fromJson<QString>(obj["original_name"]),
			   Marshalling::fromJson<QString>(obj["description"]),
			   ResourcePath::fromDocRootAbsolute(obj["path"].toString()),
			   Marshalling::fromJson<QDateTime>(obj["uploaded"].toObject()),
			   obj["width"].toInt(), obj["height"].toInt());
}
}  // namespace d3picko
