#include "include/algorithms/helper.h"
#include <opencv2/core/hal/interface.h>
#include <QDebug>
#include <QtGlobal>
#include <algorithm>
#include <numeric>
#include <set>
#include "include/colony.hpp"

namespace c3picko {
namespace math {
bool LineLineIntersect(double x1, double y1, double x2, double y2, double x3,
                       double y3, double x4, double y4, double& ixOut,
                       double& iyOut)  // Output
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
  if (denom == 0.0)  // Lines don't seem to cross
  {
    return false;
  }

  ixOut = xnom / denom;
  iyOut = ynom / denom;
  if (!std::isfinite(ixOut) ||
      !std::isfinite(iyOut))  // Probably a numerical issue
    return false;

  return true;  // All OK
}

double det(double a, double b, double c, double d) { return a * d - b * c; }

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
  // collisions(x) is the number of colonies that this colony x collides with
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

}  // namespace math
}  // namespace c3picko
