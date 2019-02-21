#include "include/types/image.hpp"
#include <QBuffer>
#include <QCryptographicHash>
#include <QDebug>
#include <QTemporaryFile>
#include <algorithm>

namespace c3picko {

Image::Image(Image::ID id, QString original_name, QString description,
             QString path, QDateTime uploaded, int width, int height)
    : original_name_(original_name), description_(description), path_(path),
      uploaded_(uploaded), width_(width), height_(height), id_(id) {
  Q_ASSERT(!id_.isEmpty());
  if (path_.isEmpty())
    qWarning() << "Image path was empty, image lost";
  Q_ASSERT(width_ && height_);
}

Image::Image(QByteArray data, QString original_name, QString description,
             QDateTime uploaded)
    : original_name_(original_name), description_(description), path_(""),
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
    : original_name_(original_name), description_(description),
      uploaded_(uploaded), image_(image), width_(image.cols),
      height_(image.rows), id_(calculateId(image_)) {
  Q_ASSERT(!id_.isEmpty());
  Q_ASSERT(path_.isEmpty());
  Q_ASSERT(width_ && height_);
}

bool Image::writeToFile() {
  if (image_.empty() ||
      path_.length()) // path_ should be empty, otherwise we may create two
                      // files with the same content
    return false;

  QTemporaryFile file(UploadFolder() + "XXXXXXXX");
  file.setAutoRemove(false);

  if (!file.open()) {
    qCritical() << "Could not write image to drive:" << file.errorString();
    return false;
  } else {
    std::vector<uint8_t> raw;
    cv::imencode(".jpg", image_, raw); // TODO use extension

    if (!file.write(reinterpret_cast<char const *>(raw.data()), raw.size())) {
      qCritical() << "Could not write image" << id_ << "(" << file.errorString()
                  << ")";
      return false;
    }

    path_ = "/" + UploadFolderName() + "/" + QFileInfo(file).fileName();
    clearCache(); // TODO make optional
    return true;
  }
}

bool Image::deleteFile() {
  if (QDir(DocRoot()).remove(path_)) {
    path_ = "";
    return true;
  }
  return false;
}

void Image::clearCache() { image_.release(); }

bool Image::crop(int x, int y, int w, int h, Image &output, QString &error) {
  // INFO in C++17 use std::clamp or qBound
  x = std::min(std::max(x, 0), width_ - 1);
  y = std::min(std::max(y, 0), height_ - 1);
  w = std::min(std::max(w, 1), width_);
  h = std::min(std::max(h, 1), height_);

  if (std::max(w, h) < 100) {
    error = "Cropped image to small";
    return false;
  }

  QString desc, name;

  desc = "Cropped version of '" + original_name_ + "'";
  name = original_name_ + "_1";

  cv::Mat image;
  if (!readCvMat(image))
    return false;
  cv::Mat cropped = image(cv::Rect(x, y, w, h)); // crop

  output = Image(cropped, name, desc, QDateTime::currentDateTime());
  return true;
}

bool Image::readCvMat(cv::Mat &output) {
  if (!(output = image_).empty())
    return true;

  QByteArray data;
  if (!readData(data))
    return false;

  return decodeCvMat(data, output);
}

bool Image::readData(QByteArray &output) const {
  output = {};
  QFile file(DocRoot() + path_.mid(1));

  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Could not open file:" << path_ << ":" << file.errorString();
    return false;
  }

  output = file.readAll();
  return true;
}

bool Image::decodeCvMat(QByteArray data, cv::Mat &output) {
  if (data.isEmpty())
    return false;

  output = cv::imdecode(cv::Mat(1, data.size(), CV_8UC1, data.data()),
                        CV_LOAD_IMAGE_UNCHANGED);
  return (!output.empty());
}

QDateTime Image::uploaded() const { return uploaded_; }

QString Image::description() const { return description_; }

Image::ID Image::calculateId(cv::Mat const &image) {
  QCryptographicHash hasher(QCryptographicHash::Sha256);

  std::vector<char> data; // TODO this is not gud
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

QString Image::path() const { return path_; }

template <> QJsonObject Marshalling::toJson(const Image &value) {
  QJsonObject obj;

  obj["id"] = value.id();
  obj["original_name"] = value.originalName();
  obj["description"] = value.description();
  obj["path"] = value.path();
  obj["uploaded"] = value.uploaded().toMSecsSinceEpoch();
  obj["width"] = value.width();
  obj["height"] = value.height();

  return obj;
}

template <> Image Marshalling::fromJson(const QJsonObject &obj) {
  return Image(obj["id"].toString(), obj["original_name"].toString(),
               obj["description"].toString(), obj["path"].toString(),
               parseDateTime(obj["uploaded"]), obj["width"].toInt(),
               obj["height"].toInt());
}
} // namespace c3picko
