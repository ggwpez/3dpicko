#include "include/algorithms/helper.h"
#include "include/colony.hpp"
#include <QDebug>
#include <QtGlobal>
#include <algorithm>
#include <numeric>
#include <opencv2/core/hal/interface.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <set>

namespace c3picko
{
namespace math
{
	bool LineLineIntersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double& ixOut,
						   double& iyOut) // Output
	{
		// http://mathworld.wolfram.com/Line-LineIntersection.html

		double detL1 = det(x1, y1, x2, y2);
		double detL2 = det(x3, y3, x4, y4);
		double x1mx2 = x1 - x2;
		double x3mx4 = x3 - x4;
		double y1my2 = y1 - y2;
		double y3my4 = y3 - y4;

		double xnom  = det(detL1, x1mx2, detL2, x3mx4);
		double ynom  = det(detL1, y1my2, detL2, y3my4);
		double denom = det(x1mx2, y1my2, x3mx4, y3my4);
		if (denom == 0.0) // Lines don't seem to cross
		{
			return false;
		}

		ixOut = xnom / denom;
		iyOut = ynom / denom;
		if (!std::isfinite(ixOut) || !std::isfinite(iyOut)) // Probably a numerical issue
			return false;

		return true; // All OK
	}

	double det(double a, double b, double c, double d) { return a * d - b * c; }

	double brightness(std::vector<cv::Point> const& contour, cv::Mat const& mat)
	{
		if (quint64(contour.size()) >= (std::numeric_limits<quint64>::max() >> 8))
		{
			qWarning() << Q_FUNC_INFO << "possible overflow, aborted";
			return 0;
		}

		quint64 sum = 0;

		for (std::size_t i = 0; i < contour.size(); ++i)
			sum += mat.at<quint8>(contour[i].y, contour[i].x);

		return (sum / 255.0) / double(contour.size());
	}

	std::vector<Colony> filterByMinDistanceSlow(std::vector<Colony> const& colonies, const int r, const int d, const int min_dist)
	{
		const std::size_t   n = colonies.size();
		std::vector<Colony> ret;

		// dist is an upper halfmatrix without diagonal, (n-1)² elements
		// (dist(x,y) < 0) <=> colonies[x] collides with colonies[y]
		cv::Mat half_dist(n, n, CV_64F);

		for (std::size_t x = 0; x < n; ++x)
		{
			for (std::size_t y = x + 1; y < n; ++y)
			{
				half_dist.at<double>(x, y) = distance(colonies[x].x(), colonies[x].y(), colonies[y].x(), colonies[y].y()) - 2 * r - d;
			}
		}

		cv::Mat dist;
		cv::transpose(half_dist, dist);
		dist += half_dist; // TODO im a lazy fuck

		// Calculates the number of collisions per colony
		// collisions(x) is the number of colonies that this colony x collides with
		std::vector<int> collisions(n, 0);

		for (std::size_t x = 0; x < n; ++x)
		{
			for (std::size_t y = 0; y < n; ++y)
			{
				if (dist.at<double>(x, y) < 0)
					++collisions[x];
			}
		}

		// As long as we have at least one collision
		std::vector<int>::iterator it;
		std::set<std::size_t>	  bads;
		while (*(it = std::max_element(collisions.begin(), collisions.end())) > 0)
		{
			// This colony produces the collision
			std::size_t bad = std::size_t(std::distance(collisions.begin(), it));

			collisions[bad] = 0;
			bads.insert(bad);

			for (std::size_t y = 0; y < n; ++y)
			{
				if (dist.at<double>(bad, y) < 0)
					--collisions[y];
			}
		}

		for (std::size_t i = 0; i < colonies.size(); ++i)
			if (bads.find(i) == bads.end())
				ret.push_back(colonies[i]);

		return ret;
	}

	cv::Mat detectPlate2(cv::Mat original, cv::Mat erroded)
	{
		int									pxl_count = original.rows * original.cols;
		cv::Mat								ret(original);
		std::vector<std::vector<cv::Point>> contours;

		cv::findContours(erroded, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

		int i = 0, good = 0;
		for (auto const& contour : contours)
		{
			std::vector<cv::Point> curve;
			double				   eps = 0.1 * cv::arcLength(contour, true); // good?
			cv::approxPolyDP(contour, curve, eps, true);
			double curve_area = cv::contourArea(curve);

			if (curve_area >= pxl_count / 2)
			{
				// cv::drawContours(ret, contours, i, cv::Scalar(0, 255, 0), 2);
				contours[i] = curve;
				cv::drawContours(ret, contours, i, cv::Scalar(0, 255, 0), 2);
				qDebug() << "Found #" << ++good << "Eps" << eps;
			}
			++i;
		}

		return ret;
	}

	cv::Mat detectPlate(cv::Mat original, cv::Mat erroded)
	{
		cv::Mat output(original);

		cv::Mat								canny_out;
		std::vector<std::vector<cv::Point>> conts;

		cv::Canny(erroded, canny_out, 3, 11);
		cv::findContours(canny_out, conts, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

		std::vector<math::LineSeg> lines;
		// Fit lines throug the contours and calculate the angle and length to image
		// coordinate system
		for (std::size_t i = 0; i < conts.size(); ++i)
		{
			cv::RotatedRect rect = cv::minAreaRect(conts[i]);

			// Check side ratio
			double r(std::min(rect.size.width, rect.size.height) / double(std::max(rect.size.width, rect.size.height)));
			if (r > .05 || std::isnan(r) || cv::arcLength(conts[i], false) < 10)
				continue;

			cv::Vec4d fit_line;
			// cv::drawContours(result, conts, i, color, 2, 8);
			cv::fitLine(conts[i], fit_line, CV_DIST_L2, 0, .01, .01);

			// Calc L2 length
			double		length = std::sqrt(std::pow(rect.size.width, 2) + std::pow(rect.size.height, 2));
			cv::Point2d v	  = cv::Point2d(fit_line[0] * length / 2, fit_line[1] * length / 2);
			// cv::fitLine matches the lines sometimes form top-to-bottom and sometimes
			// from bottom-to-top
			// so we need to turn the wrong ones around.
			if (fit_line[1] < 0)
			{
				v			= -v;
				fit_line[1] = -fit_line[1];
			}

			cv::Point2d start = cv::Point2d(fit_line[2], fit_line[3]) - v;
			cv::Point2d send  = cv::Point2d(fit_line[2], fit_line[3]) + v;

			math::LineSeg line{start.x, start.y, send.x, send.y, {fit_line[0], fit_line[1]}, length};
			lines.push_back(line);
		}

		std::vector<cv::Scalar>  bin_clrs;
		cv::Mat					 labels;
		std::vector<cv::Point2f> centers;
		// kmeans only works with floats
		std::vector<cv::Point2f> points(lines.size(), cv::Point2f());
		cv::TermCriteria		 crit(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1);

		for (std::size_t i = 0; i < lines.size(); ++i)
		{
			// Order lines by their rotation angle
			cv::Point2d p(-lines[i].angle.imag(), lines[i].angle.real());
			// std::cout << (std::arg(lines[i].angle) / M_PI) * 180.f << "°\n";
			points[i] = p;
			cv::line(output, cv::Point(output.cols / 2, output.rows / 2),
					 cv::Point2f(output.cols / 2 + p.x * 1000, output.rows / 2 + p.y * 1000), cv::Scalar::all(255), 1);
		}

		int const ks = 3;
		// We need atleast as many points as possible centers
		if (points.size() < ks)
		{
			throw std::runtime_error("Not enought edges found");
		}
		cv::kmeans(points, ks, labels, crit, 3, cv::KMEANS_PP_CENTERS, centers);

		for (int i = 0; i < ks; ++i)
			bin_clrs.push_back(cv::Scalar(rand() % 255, rand() % 255, rand() % 255));

		cv::circle(output, cv::Point2f(output.cols / 2, output.rows / 2), 10, cv::Scalar::all(255), 4);
		for (auto const& center : centers)
		{
			cv::line(output, cv::Point2f(output.cols / 2, output.rows / 2),
					 cv::Point2f(output.cols / 2, output.rows / 2) + cv::Point2f(center.x * 60, center.y * 60), cv::Scalar(0, 0, 255), 3);
			// std::cout << "c " << center.x << " / " << center.y << "\n";
		}

		for (std::size_t i = 0; i < lines.size(); ++i)
		{
			cv::line(output, cv::Point(lines[i].x, lines[i].y), cv::Point(lines[i].endx, lines[i].endy), cv::Scalar(0, 255, 0), 2);
		}

		return output;
	}

} // namespace math
} // namespace c3picko
