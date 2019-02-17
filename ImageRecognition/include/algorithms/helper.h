#pragma once

#include <complex>

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
}
}
