#pragma once

#include <complex>
#include <opencv2/core.hpp>

class QString;
namespace c3picko {
class Colony;
namespace math {
typedef double UnitValue;  // value in range [0,1]
typedef std::array<cv::Point, 4> OuterBorder;
typedef std::array<cv::Point, 6> InnerBorder;

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

  template <typename S>
  inline bool contains(S const& value) const {
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

  template <typename S>
  inline bool excludes(S const& value) const {
    return !contains(value);
  }

  T lower_, upper_;
  short _pad;
  bool lower_closed_, upper_closed_;
  int _pad2;
};

QString rangeToString(math::Range<double> const& v);

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
 * @brief Compute A1 and H1 well positions.
 * @return The indices in outer_border
 */
cv::Point2d gravityCenter(cv::InputArray poly);

/**
 * @brief NOTE takes the distance between the borders, not the centers.
 */
std::vector<Colony> filterByMinDistanceSlow(const std::vector<Colony>& colonies,
                                            const int r, const int d,
                                            const int min_dist);

/**
 * @brief Used for edge detection in the plate algorithm
 */
void findConnectedComponentEdges(const cv::Mat& input,
                                 std::vector<std::vector<cv::Point>>& contours,
                                 math::Range<int> const& area);

void drawText(cv::Mat& output, cv::Point pos, QString string,
              cv::Scalar color = cv::Scalar::all(255), double scale = 1,
              int thickness = 1);
}  // namespace math
}  // namespace c3picko
#include <QMetaType>
Q_DECLARE_METATYPE(c3picko::math::Range<double>);
