#include "include/types/image.h"
#include <QBuffer>
#include <QCryptographicHash>
#include <QDebug>
#include <QTemporaryFile>
#include <algorithm>
#include "include/resource_path.h"

namespace c3picko {

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
  // Calculate size
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
	  !path_.isEmpty())  // path_ should be empty, otherwise we may create two
						 // files with the same content
	return false;

  QTemporaryFile file((UploadFolder() + "XXXXXXXX").toSystemAbsolute());
  file.setAutoRemove(false);  // TODO

  if (!file.open()) {
	qCritical() << "Could not write image to drive:" << file.errorString()
				<< "Directory:" << UploadFolder().toSystemAbsolute();
	return false;
  } else {
	std::vector<uint8_t> raw;
	cv::imencode(".jpg", image_, raw);  // TODO use extension

	if (!file.write(reinterpret_cast<char const*>(raw.data()), raw.size())) {
	  qCritical() << "Could not write image" << id_ << "(" << file.errorString()
				  << ")";
	  return false;
	}

	path_ = UploadFolder() + QFileInfo(file).fileName();
	clearCache();  // TODO make optional
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

bool Image::crop(int x, int y, int w, int h, Image& output, QString& error) {
  x = qBound(0, x, width_ - 1);
  y = qBound(0, y, height_ - 1);
  w = qBound(1, w, width_ - x);
  h = qBound(1, h, height_ - y);  // TODO is this correct?

  if (std::max(w, h) < 100) {
	error = "Cropped image to small";
	return false;
  }

  QString desc, name;

  desc = "Cropped version of '" + original_name_ + "'";
  name = original_name_ + "_1";

  cv::Mat image;
  if (!readCvMat(image)) return false;
  cv::Mat cropped = image(cv::Rect(x, y, w, h));  // crop

  output = Image(cropped, name, desc, QDateTime::currentDateTime());
  return true;
}

bool Image::readCvMat(cv::Mat& output) {
  if (!(output = image_).empty()) return true;

  QByteArray data;
  if (!readData(data)) return false;

  return decodeCvMat(data, output);
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
}  // namespace c3picko
