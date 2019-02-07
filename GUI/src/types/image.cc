#include "include/types/image.hpp"
#include <QBuffer>
#include <QCryptographicHash>
#include <QDebug>
#include <QTemporaryFile>

namespace c3picko
{

Image::Image() : JsonConstructable(QJsonObject()) {}

Image::Image(const QJsonObject& obj)
	: JsonConstructable(obj), id_(obj["id"].toString()), original_name_(obj["original_name"].toString()),
	  description_(obj["description"].toString()), path_(obj["path"].toString()), uploaded_(parseDateTime(obj["uploaded"])),
	  width_(obj["width"].toInt()), height_(obj["height"].toInt())
{
	Q_ASSERT(!id_.isEmpty());
	qWarning() << "Image path was empty, image lost";
	Q_ASSERT(width_ && height_);
}

Image::Image(QByteArray data, QString original_name, QString description, QDateTime uploaded)
	: JsonConstructable(QJsonObject()), original_name_(original_name), description_(description), path_(""), uploaded_(uploaded)
{
	// Calculate size
	if (! decodeCvMat(data, image_))
	{
		qWarning() << "Loading image failed:" << original_name;
		return;
	}
	width_  = image_.cols;
	height_ = image_.rows;
	id_		= calculateId(image_);

	Q_ASSERT(!id_.isEmpty());
	Q_ASSERT(width_ && height_);
}

Image::Image(cv::Mat image, QString original_name, QString description, QDateTime uploaded)
	: JsonConstructable(QJsonObject()), original_name_(original_name), description_(description), uploaded_(uploaded),
	  image_(image), width_(image.cols), height_(image.rows), id_(calculateId(image_))
{
	Q_ASSERT(!id_.isEmpty());
	Q_ASSERT(path_.isEmpty());
	Q_ASSERT(width_ && height_);
}

bool Image::writeToFile()
{
	if (image_.empty() || path_.length()) // path_ should be empty, otherwise we may create two files with the same content
		return false;

	QTemporaryFile file(UploadFolder() + "XXXXXXXX");
	file.setAutoRemove(false);

	if (!file.open())
	{
		qCritical() << "Could not write image to drive:" << file.errorString();
		return false;
	}
	else
	{
		std::vector<uint8_t> raw;
		cv::imencode(".jpg", image_, raw); // TODO use extension

		if (!file.write(reinterpret_cast<char const*>(raw.data()), raw.size()))
		{
			qCritical() << "Could not write image" << id_ << "\n" << file.errorString();
			return false;
		}

		path_ = "/"+ UploadFolderName() + "/" + QFileInfo(file).fileName();
		clearCache(); // TODO make optional
		return true;
	}
}

bool Image::deleteFile()
{
	if (QDir(DocRoot()).remove(path_))
	{
		path_ = "";
		return true;
	}
	return false;
}

void Image::clearCache() { image_.release(); }

bool Image::crop(int x, int y, int w, int h, Image& output)
{
	QString desc, name;

	desc = "Cropped version of '" + original_name_ + "'";
	name = original_name_ + "_1";

	cv::Mat image;
	if (! readCvMat(image))
		return false;
	cv::Mat					 cropped = image(cv::Rect(x, y, w, h)); // crop

	output = Image(cropped, name, desc, QDateTime::currentDateTime());
	return true;
}

bool Image::readCvMat(cv::Mat& output)
{
	if (!(output = image_).empty())
		return true;

	QByteArray data;
	if (! readData(data))
		return false;

	return decodeCvMat(data, output);
}

bool Image::readData(QByteArray& output) const
{
	output = {};
	QFile file(DocRoot() +path_.mid(1));

	if (!file.open(QIODevice::ReadOnly))
	{
		qWarning() << "Could not open file:" << path_ << ":" << file.errorString();
		return false;
	}

	output = file.readAll();
	return true;
}

bool Image::decodeCvMat(QByteArray data, cv::Mat& output)
{
	if (data.isEmpty())
		return false;

	output = cv::imdecode(cv::Mat(1, data.size(), CV_8UC1, data.data()), CV_LOAD_IMAGE_UNCHANGED);
	return (! output.empty());
}

Image::ID Image::calculateId(cv::Mat const& image)
{
	QCryptographicHash hasher(QCryptographicHash::Sha256);

	std::vector<char> data; // TODO this is not gud
	for (int i = 0; i < image.rows; ++i)
		data.insert(data.end(), image.ptr<uint8_t>(i), image.ptr<uint8_t>(i) + image.cols);

	hasher.addData(data.data(), data.size());
	return hasher.result().toHex();
}

Image::ID Image::id() const { return id_; }

int Image::width() const { return width_; }

int Image::height() const { return height_; }

QString Image::originalName() const { return original_name_; }

QString Image::path() const { return path_; }

void Image::write(QJsonObject& obj) const
{
	obj["id"]			 = id_;
	obj["original_name"] = original_name_;
	obj["description"]   = description_;
	obj["path"]			 = path_;
	obj["uploaded"]		 = uploaded_.toMSecsSinceEpoch();
	obj["width"]		 = width_;
	obj["height"]		 = height_;
}
}
