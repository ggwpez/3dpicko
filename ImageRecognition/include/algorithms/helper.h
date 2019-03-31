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
  inline Range()
      : lower_(T()), upper_(T()), lower_closed_(true), upper_closed_(true) {}

  inline Range(const Range<T>& other)
      : lower_(other.lower_),
        upper_(other.upper_),
        lower_closed_(other.lower_closed_),
        upper_closed_(other.upper_closed_) {}
  /**
   * @brief Range Contructor.
   */
  Range(const T& lower, const T& upper, bool lower_closed = true,
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
  template <typename S = T>
  bool contains(S const& value) const {
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
  template <typename S = T>
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
 * @brief Convert a range to a string.
 */
QString rangeToString(math::Range<double> const& v);

/**
 * @brief Converts an Image to a base64 Png string für web application use.
 * @return Base64 PNG data.
 */
QByteArray matToBase64(cv::Mat const&);

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

/**
 * @brief Draws @a string on @a out at position @pos with font @color, font
 * @scale and line @thickness
 * @param output Output image.
 * @param pos Position of the text.
 */
void drawText(cv::Mat& output, cv::Point pos, QString string,
              cv::Scalar color = cv::Scalar::all(255), double scale = 1,
              int thickness = 1);
}  // namespace math
}  // namespace c3picko
#include <QMetaType>
template struct c3picko::math::Range<double>;
// This forward declaration works, but the compiler does not create linkage?
// TODO readelf -s --wide helper.o | xargs -I% sh -c "echo % | grep -o '[^ ]*$'
// | grep -i 'range' | c++filt" template <> template <> bool
// c3picko::math::Range<double>::contains<double>(double const&) const; template
// <> template <> bool c3picko::math::Range<double>::excludes<double>(double
// const&) const;
Q_DECLARE_METATYPE(c3picko::math::Range<double>);
