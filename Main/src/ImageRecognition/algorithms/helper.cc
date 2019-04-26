#include "ImageRecognition/algorithms/helper.h"
#include "ImageRecognition/colony.h"

#include <QByteArray>
#include <QDebug>
#include <QtGlobal>

#include <algorithm>
#include <complex>
#include <numeric>
#include <set>

#include <opencv2/core/hal/interface.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

namespace c3picko {
namespace math {

double brightness(std::vector<cv::Point> const& contour, cv::Mat const& mat) {
  if (quint64(contour.size()) >= (std::numeric_limits<quint64>::max() >> 8)) {
	qWarning() << Q_FUNC_INFO << "possible overflow, aborted";
	return 0;
  }

  quint64 sum = 0;

  for (std::size_t i = 0; i < contour.size(); ++i)
	sum += mat.at<quint8>(contour[i].y, contour[i].x);

  return (sum / 255.0) / double(contour.size());
}

cv::Point2d gravityCenter(cv::InputArray poly) {
  cv::Moments m = cv::moments(poly);

  if (!m.m00 || std::isnan(m.m00))
	throw std::runtime_error("Cant find gravity center");

  return cv::Point2d(m.m10 / m.m00, m.m01 / m.m00);
}

void drawText(cv::Mat& output, cv::Point pos, QString string, cv::Scalar color,
			  double scale, int thickness) {
  cv::Point2i img_center{output.cols / 2, output.rows / 2};

  int f_type = cv::FONT_HERSHEY_SIMPLEX;
  double f_scale = scale;
  int f_thick = thickness;

  std::string text = string.toStdString();

  cv::Size text_size = cv::getTextSize(text, f_type, f_scale, f_thick, nullptr);
  cv::Point t_pos = {pos.x - text_size.width / 2, pos.y + text_size.height / 2};
  cv::putText(output, text, t_pos, f_type, f_scale, color, f_thick);
}

std::vector<Colony> filterByMinDistanceSlow(std::vector<Colony> const& colonies,
											const int r, const int d,
											const int min_dist) {
  const std::size_t n = colonies.size();
  std::vector<Colony> ret;

  // dist is an upper halfmatrix without diagonal, (n-1)² elements
  // (dist(x,y) < 0) <=> colonies[x] collides with colonies[y]
  cv::Mat half_dist(n, n, CV_64F);

  for (std::size_t x = 0; x < n; ++x) {
	for (std::size_t y = x + 1; y < n; ++y) {
	  half_dist.at<double>(x, y) = distance(colonies[x].x(), colonies[x].y(),
											colonies[y].x(), colonies[y].y()) -
								   2 * r - d;
	}
  }

  cv::Mat dist;
  cv::transpose(half_dist, dist);
  dist += half_dist;  // TODO im a lazy fuck

  // Calculates the number of collisions per colony
  // collisions[x] is the number of colonies that this colony x collides with
  std::vector<int> collisions(n, 0);

  for (std::size_t x = 0; x < n; ++x) {
	for (std::size_t y = 0; y < n; ++y) {
	  if (dist.at<double>(x, y) < 0) ++collisions[x];
	}
  }

  // As long as we have at least one collision
  std::vector<int>::iterator it;
  std::set<std::size_t> bads;
  while (*(it = std::max_element(collisions.begin(), collisions.end())) > 0) {
	// This colony produces the collision
	std::size_t bad = std::size_t(std::distance(collisions.begin(), it));

	collisions[bad] = 0;
	bads.insert(bad);

	for (std::size_t y = 0; y < n; ++y) {
	  if (dist.at<double>(bad, y) < 0) --collisions[y];
	}
  }

  for (std::size_t i = 0; i < colonies.size(); ++i)
	if (bads.find(i) == bads.end()) ret.push_back(colonies[i]);

  return ret;
}

std::vector<std::vector<cv::Point>> findConnectedComponentEdges(
	cv::Mat const& input, std::vector<std::vector<cv::Point>>& contours,
	math::Range<int> const& area, double rel_eps) {
  std::vector<std::vector<cv::Point>> components;
  // TODO filter area with stats
  cv::Mat stats, centers, labels(input.rows, input.cols, CV_32S);

  cv::connectedComponentsWithStats(input, labels, stats, centers, 8);
  auto it(std::max_element(labels.begin<int>(), labels.end<int>()));
  // Is the matrix empty?
  if (it == labels.end<int>() || *it < 1) return {};
  components.resize(std::size_t(*it));

  for (int r = 0; r < input.rows; ++r) {
	for (int c = 0; c < input.cols; ++c) {
	  int label = labels.at<int>(r, c);
	  // 0 is the background label
	  if (!label--) continue;

	  // Save the contour point
	  components[label].emplace_back(c, r);
	}
  }

  std::vector<std::vector<cv::Point>> ret;
  // Approximate the contours with a variable epsilon
  for (std::size_t i = 0; i < components.size(); ++i) {
	std::vector<cv::Point> hull;
	cv::convexHull(components[i], hull, true);
	if (area.excludes(cv::contourArea(hull))) continue;

	double abs_eps = rel_eps * cv::arcLength(hull, true);
	contours.emplace_back();
	cv::approxPolyDP(hull, contours.back(), abs_eps, true);
	ret.emplace_back(std::move(components[i]));
  }

  return ret;
}

cv::Point2d gravityCenter(std::vector<cv::Point> const& poly) {
  cv::Moments m = cv::moments(poly);

  if (!m.m00 || std::isnan(m.m00))
	throw std::runtime_error("Cant find gravity center");

  return cv::Point2d(m.m10 / m.m00, m.m01 / m.m00);
}

QString rangeToString(const math::Range<double>& v) {
  QString begin = (v.lower_closed_ ? "[" : "("),
		  end = (v.upper_closed_ ? "]" : ")");

  return begin + QString::number(v.lower_) + ", " + QString::number(v.upper_) +
		 end;
}

QByteArray matToBase64(const cv::Mat& mat) {
  QByteArray data;
  std::vector<uchar> tmp;
  cv::imencode(".png", mat, tmp);
  std::copy(tmp.begin(), tmp.end(), std::back_inserter(data));

  return data.toBase64();
}
}  // namespace math
}  // namespace c3picko
