#include "include/algorithms/plate1.h"
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
                {}, true, 5000) {}

void Plate1::cvt(AlgorithmJob* base, PlateResult* result) {
  cv::Mat& input = *reinterpret_cast<cv::Mat*>(base->input());
  cv::Mat& output = result->newMat();

  cv::cvtColor(input, output, CV_BGR2GRAY);
}
void Plate1::threshold(AlgorithmJob*, PlateResult* result) {
  cv::Mat& input = result->oldMat();
  cv::Mat& output = result->newMat();

  cv::Mat tmp, _;
  double otsu =
      cv::threshold(input, _, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  cv::Canny(input, tmp, otsu / 2, otsu);

  cv::GaussianBlur(tmp, tmp, cv::Size(3, 3), 0);
  int erosion_type = cv::MORPH_RECT;
  int erosion_size = 1;
  cv::Mat kernel = getStructuringElement(
      erosion_type, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
      cv::Point(erosion_size, erosion_size));
  cv::dilate(tmp, output, kernel);
}

// Strict include, points on the edge are considered outside
template <std::size_t s1, std::size_t s2>
bool polyIncludesPoly(std::array<cv::Point, s1> const& poly1,
                      std::array<cv::Point, s2> const& poly2) {
  for (std::size_t i = 0; i < poly2.size(); ++i)
    if (cv::pointPolygonTest(poly1, poly2[i], false) <= 0) return false;

  return true;
}

void Plate1::detect(AlgorithmJob* base, PlateResult* result) {
  cv::Mat const& original = *reinterpret_cast<cv::Mat*>(base->input());
  cv::Mat const& erroded = result->oldMat();

  math::Range<int> area((original.rows * original.cols) / 32,
                        std::numeric_limits<int>::max());
  cv::Mat& ret(result->newMat(original));
  std::vector<math::OuterBorder> outer_edges;
  std::vector<math::InnerBorder> inner_edges;
  std::vector<std::vector<cv::Point>> contours;

  math::findConnectedComponentEdges(erroded, contours, area);
  qDebug() << "Found" << contours.size() << "contours";

  for (std::size_t i = 0; i < contours.size(); ++i) {
    auto curve = contours[i];
    /*std::vector<cv::Point> curve;
    double				   eps = 0.05 * cv::arcLength(contours[i],
    true);
    // good? cv::approxPolyDP(contours[i], curve, eps, true);*/
    double a = cv::contourArea(curve);

    /*cv::Mat tmp;
    cv::resize(ret, tmp, cv::Size(ret.cols / 2, ret.rows / 2));
    cv::imshow(std::to_string(i), tmp);
    cv::waitKey(0);
    cv::destroyAllWindows();*/

    if (area.excludes(a)) continue;
    // Filter out all curves with 4 or 6 points and size atleast 1/16 or the
    // image size
    if ((curve.size() == 6 || curve.size() == 4)) {
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
    throw std::runtime_error("Could not approximate outer edge");
  if (!inner_edges.size())
    throw std::runtime_error("Could not approximate inner edge");

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

  result->original_ = Plate(outer_edge, inner_edge);
  result->rotated_ = result->original_.normalized(ret, ret);
}
}  // namespace c3picko
