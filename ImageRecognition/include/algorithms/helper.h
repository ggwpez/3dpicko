#pragma once

#include <complex>
#include <opencv2/core.hpp>

namespace c3picko
{
namespace math
{
	struct LineSeg
	{
		double x, y;
		double endx, endy;
		/**
		 * @brief Angle Relative to the image coordinate system.
		 */
		std::complex<double> angle;
		double				 length;
	};

	template <typename T> struct Range
	{
		inline Range(T const& lower, T const& upper, bool lower_closed = true, bool upper_closed = true)
			: lower_(lower), upper_(upper), lower_closed_(lower_closed), upper_closed_(upper_closed)
		{
		}

		inline bool contains(T const& value)
		{
			bool ok = !std::isnan(value);

			if (lower_closed_)
				ok &= value >= lower_;
			else
				ok &= value > lower_;

			if (upper_closed_)
				ok &= value <= upper_;
			else
				ok &= value < upper_;

			return ok;
		}

		inline bool excludes(T const& value) { return !contains(value); }

		bool lower_closed_, upper_closed_;
		T	lower_, upper_;
	};

	/**
	 * @brief Determinant of the matrix
	 * a b
	 * c d
	 */
	double det(double a, double b, double c, double d);

	/**
	 * @brief L2-Norm of (x1,y1) and (x2,y2)
	 */
	double distance(double x1, double y1, double x2, double y2);

	/**
	 * @brief LineLineIntersect
	 */
	bool LineLineIntersect(double x1, double y1, // Line 1 start
						   double x2, double y2, // Line 1 end
						   double x3, double y3, // Line 2 start
						   double x4, double y4, // Line 2 end
						   double& ixOut, double& iyOut);

	double brightness(std::vector<cv::Point> const& contour, cv::Mat const& mat);
}
}
