#include "include/algorithms/plate.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include "include/algorithm_job.h"
#include "include/algorithms/normal1.h"
#include "include/plate_result.h"

namespace c3picko {
Plate1::Plate1()
    : Algorithm("1", "Plate1", "Detects a plate with red frame attached",
                {(AlgoStep)Plate1::cvt, (AlgoStep)Plate1::threshold,
                 (AlgoStep)Plate1::detect},
                {}, false, 5000) {}

void Plate1::cvt(AlgorithmJob* base, PlateResult* result) {
  cv::Mat& input = *reinterpret_cast<cv::Mat*>(base->input());
  cv::Mat& output = result->newMat();

  cv::cvtColor(input, output, CV_BGR2GRAY);
}
void Plate1::threshold(AlgorithmJob*, PlateResult* result) {
  cv::Mat& input = result->oldMat();
  cv::Mat& output = result->newMat();

  cv::GaussianBlur(input, output, cv::Size(5, 5), 0);
  cv::adaptiveThreshold(output, output, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                        cv::THRESH_BINARY_INV, 51, 1);  // non flour
  cv::Canny(output, output, 3, 11);
  cv::GaussianBlur(output, output, cv::Size(3, 3), 0);
}

// Strict include, points on the edge are considered outside
template <std::size_t s1, std::size_t s2>
bool polyIncludesPoly(std::array<cv::Point, s1> const& poly1,
                      std::array<cv::Point, s2> const& poly2) {
  for (std::size_t i = 0; i < poly2.size(); ++i)
    if (cv::pointPolygonTest(poly1, poly2[i], false) <= 0) return false;

  return true;
}

// Calculates the rotation of a quasi rectangle, only works counter clockwise
double calculateOuterRotation(std::vector<cv::Point> const& cont, int a1,
                              int h1) {
  Q_ASSERT(cont.size() == 4);

  // There sure is an easier way but for now:
  // Convert all angles to polarcoordinates with magnitude = 1, average them and
  // take the resulting phase angle
  auto c1 = std::polar(
      1., std::atan2(cont[h1].y - cont[a1].y, cont[h1].x - cont[a1].x));
  auto c2 = std::polar(1., std::atan2(cont[(h1 + 1) % 4].y - cont[h1].y,
                                      cont[(h1 + 1) % 4].x - cont[h1].x) +
                               M_PI_2);
  auto c3 =
      std::polar(1., std::atan2(cont[(h1 + 2) % 4].y - cont[(h1 + 1) % 4].y,
                                cont[(h1 + 2) % 4].x - cont[(h1 + 1) % 4].x) +
                         M_PI);
  auto c4 =
      std::polar(1., std::atan2(cont[(h1 + 3) % 4].y - cont[(h1 + 2) % 4].y,
                                cont[(h1 + 3) % 4].x - cont[(h1 + 2) % 4].x) -
                         M_PI_2);

  auto sum = (c1 + c2 + c3 + c4) / 4.;  // the division should be unnecessary

  double ret = std::arg(sum) / M_PI * 180;
  return ret < 0 ? ret + 360 : ret;
}

// this function assumes, that OpenCV creates contours counter clockwise
// and that the image was not mirrored
std::pair<int, int> findA1(std::vector<cv::Point> const& outer_border,
                           std::vector<cv::Point> const& inner_border) {
  Q_ASSERT(inner_border.size() == 6 && outer_border.size() == 4);

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
  if ((a1 + 1) % outer_border.size() != h1) std::swap(a1, h1);
  Q_ASSERT((a1 + 1) % outer_border.size() == h1);

  if (a1 == -1 || h1 == -1 || a1 == h1)
    throw std::runtime_error("Could not detect orientation of plate");

  return {a1, h1};
}

void Plate1::detect(AlgorithmJob* base, PlateResult* result) {
  cv::Mat const& original = *reinterpret_cast<cv::Mat*>(base->input());
  cv::Mat const& erroded = result->oldMat();

  int min_pxl_count = (original.rows * original.cols) / 32;
  cv::Mat& ret(result->newMat(original));
  std::vector<math::OuterBorder> outer_edges;
  std::vector<math::InnerBorder> inner_edges;
  std::vector<std::vector<cv::Point>> contours;

  cv::findContours(erroded, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

  for (int i = 0; i < contours.size(); ++i) {
    std::vector<cv::Point> curve;
    double eps = 0.01 * cv::arcLength(contours[i], true);  // good?
    cv::approxPolyDP(contours[i], curve, eps, true);
    double area = cv::contourArea(curve);

    // Filter out all curves with 4 or 6 points and size atleast 1/16 or the
    // image size
    if ((curve.size() == 6 || curve.size() == 4) && area >= min_pxl_count) {
      if (curve.size() == 6) {
        math::InnerBorder edge;
        std::copy(curve.begin(), curve.begin() + 6, edge.begin());
        inner_edges.push_back(edge);
      } else {
        math::OuterBorder edge;
        std::copy(curve.begin(), curve.begin() + 4, edge.begin());
        outer_edges.push_back(edge);
      }
    }
  }

  if (!outer_edges.size())
    throw std::runtime_error("Could not approximate outer edges");
  if (!inner_edges.size())
    throw std::runtime_error("Could not approximate inner edges");

  double eps = .1;
  double plate_ratio = 128. / 85.9;  // FIXME get data from plate profile
  double optimal = 1 / plate_ratio;
  math::Range<double> outer_bb_ratio(optimal - eps, optimal + eps);

  // Now filter the found curves to find the ones belonging to the plate
  // Filter 4p polygons by bb side ratio
  for (auto it = outer_edges.begin(); it != outer_edges.end();) {
    auto edge(*it);

    double w = math::distance(edge[0].x, edge[0].y, edge[1].x, edge[1].y);
    double h = math::distance(edge[1].x, edge[1].y, edge[2].x,
                              edge[2].y);  // TODO use cv::norm

    double r(std::min(w, h) / double(std::max(w, h)));
    if (!outer_bb_ratio.contains(r))
      outer_edges.erase(it);
    else
      ++it;
  }

  if (!outer_edges.size())
    throw std::runtime_error("Could not approximate outer edges");

  // Filter 6p polygons
  // Distances between the 4p's and 6p's to find the two, that are closest
  // together and where the 4p strictly includes the 6p first int is the
  // inner_edge index second one the outer
  std::vector<std::tuple<int, int, double>> corner_distances;
  for (std::size_t i = 0; i < inner_edges.size(); ++i) {
    auto inner(inner_edges[i]);

    for (std::size_t j = 0; j < outer_edges.size(); ++j) {
      auto outer(outer_edges[j]);
      // Sum of Squared error TODO right now its only the distance, but it
      // should be an error therefore the optimal width of the frame must be
      // calculated and compared to the current width
      double corner_error = 0;

      // If the outer_edge that is not outside of this inner_edge, its an error
      // It would mean, that the 6p is outside the 4p, which is not how a plate
      // looks like NOTE if we kick out a correct 4p here, try to decrease the
      // eps for the approxPolyDP
      if (!polyIncludesPoly(outer, inner)) continue;

      for (std::size_t c = 0; c < outer.size(); ++c)  // 4 loops, RIP me
      {
        // Minimum distance between the two closest points from 4p and 6p
        double d_min = std::numeric_limits<double>::infinity();
        // index of the d_min element
        int r_min = -1;

        for (std::size_t r = 0; r < inner.size(); ++r) {
          double d = cv::norm(inner[r] - outer[c]);
          if (d < d_min) {
            r_min = r;
            d_min = d;
          }
        }

        if (r_min == -1)
          throw std::runtime_error("Could detect rotation of inner edges");
        corner_error += std::pow(d_min, 2);
      }

      corner_distances.emplace_back(i, j, corner_error);
    }
  }

  // Now filter for the best match by the Sum of Squared Error
  auto it = std::min_element(corner_distances.begin(), corner_distances.end());
  if (it == corner_distances.end())
    throw std::runtime_error("Could detect rotation of inner edges");

  auto outer_edge = outer_edges[std::get<0>(*it)];
  auto inner_edge = inner_edges[std::get<1>(*it)];
  double best_dist = std::get<2>(*it);
  // cv::Point2d inner_center = gravityCenter(inner_edge.curve), outer_center =
  // gravityCenter(outer_edge.curve); cv::Point2d center = (inner_center +
  // outer_center) / 2;

  Plate plate(outer_edge, inner_edge);
  Plate normalized = plate.normalized(ret, ret);

  math::drawText(ret, outer_edge[plate.a1()], "A1");
  math::drawText(ret, outer_edge[plate.h1()], "H1");
  for (int i = 0; i < outer_edge.size(); ++i)
    math::drawText(ret, outer_edge[i], QString::number(i));

  // draw borders
  /*cv::drawContours(ret, std::vector<std::vector<cv::Point>>{inner_edge.curve},
  0, cv::Scalar(0, 255, 0), 2); cv::drawContours(ret,
  std::vector<std::vector<cv::Point>>{outer_edge.curve}, 0, cv::Scalar(0, 255,
  0), 2);

  // draw center
  cv::ellipse(ret, center, cv::Size(20, 20), 90, 0, 360, cv::Scalar(255, 0, 0),
  2); cv::ellipse(ret, outer_center, cv::Size(20, 20), 90, 0, 180, cv::Scalar(0,
  255, 0), 2); cv::ellipse(ret, inner_center, cv::Size(20, 20), 90, 180, 360,
  cv::Scalar(0, 0, 255), 2);

  // draw A1 and H1
  std::pair<int, int> a1_h1 = findA1(outer_edge.curve, inner_edge.curve);
  math::drawText(ret, outer_edge.curve[a1_h1.first], "A1");
  math::drawText(ret, outer_edge.curve[a1_h1.second], "H1");

  double angle = calculateOuterRotation(outer_edge.curve, a1_h1.first,
  a1_h1.second);

  qDebug() << "\nFound borders, dist=" << best_dist << "Rotation" << angle << "°
  Center" << inner_center.x << inner_center.y << "Diff"
                   << cv::norm(inner_center - outer_center) << "px";

  // Rotate the image and curves
  cv::Mat rotation = cv::getRotationMatrix2D(center, angle - 90, 1);
  cv::warpAffine(ret, ret, rotation, cv::Size(ret.cols, ret.rows));
  cv::transform(outer_edge.curve, outer_edge.curve, rotation);
  cv::transform(inner_edge.curve, inner_edge.curve, rotation);

  // Cut it
  ret				 = ret(cv::boundingRect(outer_edge.curve)); //
  lazy result->newMat() = ret;*/
}
}  // namespace c3picko
