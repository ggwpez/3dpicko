#pragma once

#include <complex>
#include <opencv2/core.hpp>

/**
 * This file is the collecting pond for all that needs to find a permanent home.
 */

class QByteArray;
class QString;
namespace c3picko {
class Colony;
namespace math {
typedef double UnitValue;  // value in range [0,1]
typedef std::array<cv::Point, 4> OuterBorder;
typedef std::array<cv::Point, 6> InnerBorder;

/**
 * @brief Represents a 2D interval in the mathematical sense.
 *
 * @tparam T Value type.
 */
template <typename T>
struct Range {
  inline Range(Range const& other)
      : lower_(other.lower_),
        upper_(other.upper_),
        lower_closed_(other.lower_closed_),
        upper_closed_(other.upper_closed_) {}
  inline Range()
      : lower_(T()), upper_(T()), lower_closed_(true), upper_closed_(true) {}
  /**
   * @brief Range Contructor.
   */
  inline Range(T const& lower, T const& upper, bool lower_closed = true,
               bool upper_closed = true)
      : lower_(lower),
        upper_(upper),
        lower_closed_(lower_closed),
        upper_closed_(upper_closed) {}

  /**
   * @return Whether value is conained on the range.
   * @tparam S Type of the value that should be checked against.
   * Needs to have operators <, <=, > and <= for (S, T)
   */
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

  /**
   * @return Whether value is excluded on the range.
   * @tparam S Type of the value that should be checked against.
   * Needs to have operators <, <=, > and <= for (S, T)
   */
  template <typename S>
  inline bool excludes(S const& value) const {
    return !contains(value);
  }

  /**
   * @brief Lower value.
   */
  T lower_;
  /**
   * @param Interval's right side is closed.
   */
  T upper_;
  /**
   * @brief Left side is closed.
   */
  bool lower_closed_;
  /**
   * @brief Right side is closed.
   */
  bool upper_closed_;
};

/**
 * @brief Converts an Image to a base64 Png string für web application use.
 * @return Base64 PNG data.
 */
QByteArray matToBase64(cv::Mat const&);

QString rangeToString(math::Range<double> const& v);

/**
 * @brief L2-Norm of (x1,y1) and (x2,y2)
 */
template <typename T>
inline T distance(T x1, T y1, T x2, T y2) {
  return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

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
