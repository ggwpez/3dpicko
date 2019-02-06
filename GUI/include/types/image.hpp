#pragma once

#include "include/json_constructable.hpp"
#include <QDateTime>
#include <QImage>
#include <memory>
#include <opencv2/opencv.hpp>

namespace c3picko
{
class Image : public JsonConstructable
{
  public:
	typedef QString ID;
	Image();
	Image(QJsonObject const&);
	Image(QByteArray data, QString original_name, QString description, QDateTime uploaded);
	Image(cv::Mat image, QString original_name, QString description, QDateTime uploaded);

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

	/**
	 * @brief Returns a cropped version of the current image.
	 * Non-const bc it may modify the cache.
	 * @param x Start x pos
	 * @param y Start y pos
	 * @param w Crop Width
	 * @param h Crop Height
	 * @return
	 */
	Image crop(int x, int y, int w, int h);

	std::shared_ptr<cv::Mat> readCvMat();
	QByteArray				 readData() const;
	static cv::Mat decodeCvMat(QByteArray data);

  private:
	QString original_name_;
	QString description_;
	// TODO we can add meta data here
	/**
	 * @brief The relative file path, not starting with /
	 */
	QString   path_;
	QDateTime uploaded_;
	/**
	 * @brief The image itself. Also referred to as the 'cache'.
	 */
	std::shared_ptr<cv::Mat> image_ = nullptr;
	int						 width_ = 0, height_ = 0;
	ID						 id_;

  public:
	QString path() const;
	ID		id() const;
	QString originalName() const;
	int		width() const;
	int		height() const;

	void write(QJsonObject&) const override;
};
}
