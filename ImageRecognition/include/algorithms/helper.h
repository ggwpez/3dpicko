#pragma once

#include <complex>
#include <opencv2/core.hpp>

namespace c3picko {
class Colony;
namespace math {
struct LineSeg {
  double x, y;
  double endx, endy;
  /**
   * @brief Angle Relative to the image coordinate system.
   */
  std::complex<double> angle;
  double length;
};

template <typename T>
struct Range {
  inline Range(Range const& other)
      : lower_(other.lower_),
        upper_(other.upper_),
        lower_closed_(other.lower_closed_),
        upper_closed_(other.upper_closed_) {}
  inline Range()
      : lower_(T()), upper_(T()), lower_closed_(true), upper_closed_(true) {}
  inline Range(T const& lower, T const& upper, bool lower_closed = true,
               bool upper_closed = true)
      : lower_(lower),
        upper_(upper),
        lower_closed_(lower_closed),
        upper_closed_(upper_closed) {}

  inline bool contains(T const& value) {
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

  T lower_, upper_;
  short _pad;
  bool lower_closed_, upper_closed_;
  int _pad2;
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
template <typename T>
inline T distance(T x1, T y1, T x2, T y2) {
  return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

/**
 * @brief LineLineIntersect
 */
bool LineLineIntersect(double x1, double y1,  // Line 1 start
                       double x2, double y2,  // Line 1 end
                       double x3, double y3,  // Line 2 start
                       double x4, double y4,  // Line 2 end
                       double& ixOut, double& iyOut);

double brightness(std::vector<cv::Point> const& contour, cv::Mat const& mat);

/**
 * @brief NOTE takes the distance between the borders, not the centers.
 */
std::vector<Colony> filterByMinDistanceSlow(const std::vector<Colony>& colonies,
                                            const int r, const int d,
                                            const int min_dist);
}  // namespace math
}  // namespace c3picko
#include <QMetaType>
Q_DECLARE_METATYPE(c3picko::math::Range<double>);
