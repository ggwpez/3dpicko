#include "include/algorithms/helper.h"
#include <QDebug>
#include <QtGlobal>
#include <numeric>

namespace c3picko {
namespace math {
bool LineLineIntersect(double x1, double y1, double x2, double y2, double x3,
                       double y3, double x4, double y4, double &ixOut,
                       double &iyOut) // Output
{
  // http://mathworld.wolfram.com/Line-LineIntersection.html

  double detL1 = det(x1, y1, x2, y2);
  double detL2 = det(x3, y3, x4, y4);
  double x1mx2 = x1 - x2;
  double x3mx4 = x3 - x4;
  double y1my2 = y1 - y2;
  double y3my4 = y3 - y4;

  double xnom = det(detL1, x1mx2, detL2, x3mx4);
  double ynom = det(detL1, y1my2, detL2, y3my4);
  double denom = det(x1mx2, y1my2, x3mx4, y3my4);
  if (denom == 0.0) // Lines don't seem to cross
  {
    return false;
  }

  ixOut = xnom / denom;
  iyOut = ynom / denom;
  if (!std::isfinite(ixOut) ||
      !std::isfinite(iyOut)) // Probably a numerical issue
    return false;

  return true; // All OK
}

double distance(double x1, double y1, double x2, double y2) {
  return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

double det(double a, double b, double c, double d) { return a * d - b * c; }

double brightness(std::vector<cv::Point> const &contour, cv::Mat const &mat) {
  if (quint64(contour.size()) >= (std::numeric_limits<quint64>::max() >> 8)) {
    qWarning() << Q_FUNC_INFO << "possible overflow, aborted";
    return 0;
  }

  quint64 sum = 0;

  for (std::size_t i = 0; i < contour.size(); ++i)
    sum += mat.at<quint8>(contour[i].y, contour[i].x);

  return (sum / 255.d) / double(contour.size());
}
}
}
