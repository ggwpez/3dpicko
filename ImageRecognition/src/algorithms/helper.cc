#include "include/algorithms/helper.h"
#include <opencv2/core/hal/interface.h>
#include <QDebug>
#include <QtGlobal>
#include <algorithm>
#include <complex>
#include <numeric>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
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

cv::Point2d gravityCenter(cv::InputArray poly) {
  cv::Moments m = cv::moments(poly);

  if (!m.m00 || std::isnan(m.m00))
    throw std::runtime_error("Cant find gravity center");

  return cv::Point2d(m.m10 / m.m00, m.m01 / m.m00);
}

void drawText(cv::Mat& output, cv::Point pos, QString string,
              cv::Scalar color) {
  cv::Point2i img_center{output.cols / 2, output.rows / 2};

  int f_type = cv::FONT_HERSHEY_SIMPLEX;
  int f_scale = 2;
  int f_thick = 2;

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

struct Curve {
  // from cv::findContours
  std::vector<cv::Point> raw;
  // approximation epsilon
  double eps;
  double area;
  // approximated
  std::vector<cv::Point> curve;
};

// Strict include, points on the edge are considered outside
bool polyIncludesPoly(std::vector<cv::Point> const& poly1,
                      std::vector<cv::Point> const& poly2) {
  for (int i = 0; i < poly2.size(); ++i)
    if (cv::pointPolygonTest(poly1, poly2[i], false) <= 0) return false;

  return true;
}

// Calculates the rotation of a quasi rectangle, only works counter clockwise
double calculateOuterRotation(const OuterBorder& cont, int a1, int h1) {
  // There sure is an easier way but for now:
  // Convert all angles to polarcoordinates with magnitude = 1, average them and
  // take the resulting phase angle

  auto c1 = std::polar(1., std::atan2(cont[(a1 + 1) % 4].y - cont[a1].y,
                                      cont[(a1 + 1) % 4].x - cont[a1].x));

  double ret = std::arg(c1) / M_PI * 180;
  return ret < 0 ? ret + 360 : ret;
  // FIXME
  /*auto c2 = std::polar(1., std::atan2(cont[(a1 + 2) % 4].y - cont[(a1 + 1) %
  4].y, cont[(a1 + 2) % 4].x - cont[(a1 + 1) % 4].x)
                                                           + M_PI_2);
  auto c3
          = std::polar(1., std::atan2(cont[(a1 + 3) % 4].y - cont[(a1 + 2) %
  4].y, cont[(a1 + 3) % 4].x - cont[(a1 + 2) % 4].x) + M_PI); auto c4 =
  std::polar(1., std::atan2(cont[(a1 + 4) % 4].y - cont[(a1 + 3) % 4].y,
  cont[(a1 + 4) % 4].x - cont[(a1 + 3) % 4].x)
                                                           - M_PI_2);

  auto sum = (c1 + c2 + c3 + c4);

  double ret = std::arg(sum) / M_PI * 180;
  return ret < 0 ? ret + 360 : ret;*/
}

cv::Point2d gravityCenter(std::vector<cv::Point> const& poly) {
  cv::Moments m = cv::moments(poly);

  if (!m.m00 || std::isnan(m.m00))
    throw std::runtime_error("Cant find gravity center");

  return cv::Point2d(m.m10 / m.m00, m.m01 / m.m00);
}

// this function assumes, that OpenCV creates contours counter clockwise
// and that the image was not mirrored
std::pair<int, int> findA1(const OuterBorder& outer_border,
                           const InnerBorder& inner_border) {
  // index of the outer_border points
  int ia1 = -1, ih1 = -1;
  // the length of the two diagonal edges
  double ia1_min = INFINITY, ih1_min = INFINITY;
  double eps_diag_edge_max = 10;

  for (int i = 0; i < inner_border.size(); ++i)
    for (int j = i + 1; j < inner_border.size(); ++j) {
      double dist = cv::norm(inner_border[i] - inner_border[j]);

      if (dist < ia1_min) {
        ia1_min = dist;
        ia1 = i;
      }
    }
  for (int i = 0; i < inner_border.size(); ++i)
    for (int j = i + 1; j < inner_border.size(); ++j) {
      double dist = cv::norm(inner_border[i] - inner_border[j]);

      if (dist < ih1_min && i != ia1) {
        ih1_min = dist;
        ih1 = i;
      }
    }

  // NOTE Try to increase eps
  if (ia1 == -1 || ih1 == -1 || ia1 == ih1)
    throw std::runtime_error("Could not detect diagonal edges");

  double a1_min = INFINITY, h1_min = INFINITY;
  int a1 = -1, h1 = -1;
  for (int i = 0; i < outer_border.size(); ++i) {
    double dist_a1 = cv::norm(outer_border[i] - inner_border[ia1]);
    double dist_h1 = cv::norm(outer_border[i] - inner_border[ih1]);

    if (dist_a1 < a1_min) {
      a1_min = dist_a1;
      a1 = i;
    }
    if (dist_h1 < h1_min) {
      h1_min = dist_h1;
      h1 = i;
    }
  }

  // swap a1 and h1 such that h1 is counter clockwise of a1
  if ((h1 + 1) % outer_border.size() != a1) std::swap(a1, h1);
  Q_ASSERT((h1 + 1) % outer_border.size() == a1);

  if (a1 == -1 || h1 == -1 || a1 == h1)
    throw std::runtime_error("Could not detect orientation of plate");

  return {a1, h1};
}

/*cv::Mat detectPlate2(cv::Mat original, cv::Mat erroded)
{
        int
pxl_count = original.rows * original.cols; cv::Mat
ret(original);
        std::vector<Curve>					outer_edges,
inner_edges; std::vector<std::vector<cv::Point>> contours;

        cv::findContours(erroded, contours, CV_RETR_LIST,
CV_CHAIN_APPROX_SIMPLE);

        for (int i = 0; i < contours.size(); ++i)
        {
                std::vector<cv::Point> curve;
                double				   eps = 0.01 *
cv::arcLength(contours[i], true); // good? cv::approxPolyDP(contours[i], curve,
eps, true); double area = cv::contourArea(curve);

                // Filter out all curves with 4 or 6 points and size atleast
1/16 or the image size if ((curve.size() == 6 || curve.size() == 4) && area >=
pxl_count / 16)
                {
                        if (curve.size() == 6)
                                inner_edges.push_back({contours[i], eps, area,
curve}); else outer_edges.push_back({contours[i], eps, area, curve});
                }
        }

        if (!outer_edges.size())
                throw std::runtime_error("Could not approximate outer edges");
        if (!inner_edges.size())
                throw std::runtime_error("Could not approximate inner edges");

        double				eps			= .1;
        double				plate_ratio = 128. / 85.9; //
FIXME get data from plate profile double				optimal
= 1 / plate_ratio; math::Range<double> outer_bb_ratio(optimal - eps, optimal +
eps);
        // Now filter the found curves to find the ones belonging to the plate

        // Filter 4p polygons by bb side ratio
        for (int i = 0; i < outer_edges.size();)
        {
                Curve edge(outer_edges[i]);

                double w = distance(edge.curve[0].x, edge.curve[0].y,
edge.curve[1].x, edge.curve[1].y); double h = distance(edge.curve[1].x,
edge.curve[1].y, edge.curve[2].x, edge.curve[2].y);

                double r(std::min(w, h) / double(std::max(w, h)));
                if (!outer_bb_ratio.contains(r))
                        outer_edges.erase(outer_edges.begin() + i);
                else
                        ++i;
        }

        // Filter 6p polygons
        // Distances between the 4p's and 6p's to find the two, that are closest
together
        // and where the 4p strictly includes the 6p
        // first int is the inner_edge index second one the outer
        std::vector<std::tuple<int, int, double>> corner_distances;
        for (int i = 0; i < inner_edges.size(); ++i)
        {
                Curve inner(inner_edges[i]);

                for (int j = 0; j < outer_edges.size(); ++j)
                {
                        Curve outer(outer_edges[j]);
                        // Sum of Squared error
                        double corner_error = 0;

                        // If the outer_edge that is not outside of this
inner_edge, its an error
                        // It would mean, that the 6p is outside the 4p, which
is not how a plate looks like
                        // NOTE if we kick out a correct 4p here, try to
decrease the eps for the approxPolyDP if (!polyIncludesPoly(outer.curve,
inner.curve)) continue;

                        for (int c = 0; c < outer.curve.size(); ++c) // 4 loops,
RIP me
                        {
                                // Minimum distance between the two closest
points from 4p and 6p double d_min = std::numeric_limits<double>::infinity();
                                // index of the d_min element
                                int r_min = -1;

                                for (int r = 0; r < inner.curve.size(); ++r)
                                {
                                        double d = cv::norm(inner.curve[r] -
outer.curve[c]); if (d < d_min)
                                        {
                                                r_min = r;
                                                d_min = d;
                                        }
                                }

                                if (r_min == -1)
                                        throw std::runtime_error("Could detect
rotation of inner edges"); corner_error += std::pow(d_min, 2);
                        }

                        corner_distances.emplace_back(i, j, corner_error);
                }
        }

        // Now filter for the best match by the Sum of Squared Error
        auto it = std::min_element(corner_distances.begin(),
corner_distances.end()); if (it == corner_distances.end()) throw
std::runtime_error("Could detect rotation of inner edges");

        Curve		outer_edge = outer_edges[std::get<0>(*it)],
inner_edge = inner_edges[std::get<1>(*it)];
        double		best_dist	= std::get<2>(*it);
        cv::Point2d inner_center = gravityCenter(inner_edge.curve), outer_center
= gravityCenter(outer_edge.curve); cv::Point2d center = (inner_center +
outer_center) / 2;

        // draw borders
        cv::drawContours(ret,
std::vector<std::vector<cv::Point>>{inner_edge.curve}, 0, cv::Scalar(0, 255, 0),
2); cv::drawContours(ret, std::vector<std::vector<cv::Point>>{outer_edge.curve},
0, cv::Scalar(0, 255, 0), 2);

        // draw center
        cv::ellipse(ret, center, cv::Size(20, 20), 90, 0, 360, cv::Scalar(255,
0, 0), 2); cv::ellipse(ret, outer_center, cv::Size(20, 20), 90, 0, 180,
cv::Scalar(0, 255, 0), 2); cv::ellipse(ret, inner_center, cv::Size(20, 20), 90,
180, 360, cv::Scalar(0, 0, 255), 2);

        // draw A1 and H1
        std::pair<int, int> a1_h1 = findA1(outer_edge.curve, inner_edge.curve);
        drawText(ret, outer_edge.curve[a1_h1.first], "A1");
        drawText(ret, outer_edge.curve[a1_h1.second], "H1");

        double angle = calculateOuterRotation(outer_edge.curve, a1_h1.first,
a1_h1.second);

        qDebug() << "\nFound borders, dist=" << best_dist << "Rotation" << angle
<< "° Center" << inner_center.x << inner_center.y
                         << "Diff" << cv::norm(inner_center - outer_center) <<
"px";

        // Rotate the image and curves
        cv::Mat rotation = cv::getRotationMatrix2D(center, angle - 90, 1);
        cv::warpAffine(ret, ret, rotation, cv::Size(ret.cols, ret.rows));
        cv::transform(outer_edge.curve, outer_edge.curve, rotation);
        cv::transform(inner_edge.curve, inner_edge.curve, rotation);

        // Cut it
        ret = ret(cv::boundingRect(outer_edge.curve)); // lazy

        return ret;
}*/
}  // namespace math
}  // namespace c3picko
