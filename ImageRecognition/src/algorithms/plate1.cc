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
                {(AlgoStep)Normal1::cvt, (AlgoStep)Plate1::threshold,
                 (AlgoStep)Plate1::detect},
                {}, true, 5000) {}

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
  // This code was written very quickly, it is therefore quite messy and needs
  // to be reworked
  /**
   * Steps:
   *
   * 1. Detect the contours of all connected components in the black and white
   *    input image. Approximate the contours with polygons, now called edge.
   * 2. Filter the edges by their size and number of edges.
   *    6 pointed ones are interpreted as inner edge,
   *    4 pointed ones are interpreted as outer edge.
   * 3. Filter outer edges by their side ratio.
   * 4. Filter inner and outer edges such that the outer edge always includes
   *    the inner edge.
   * 5. Order all (inner,outer) pairs by their squared point distance.
   * 6. Select the pair with the smallest distance.
   * 7. Create a Plate object and a rotated instance, cut and rotate the
   *    original according to the rotated plate, such that the plate is centered
   *    in the center.
   */

  cv::Mat const& original = *reinterpret_cast<cv::Mat*>(base->input());
  cv::Mat const& erroded = result->oldMat();

  math::Range<int> area((original.rows * original.cols) / 32,
                        std::numeric_limits<int>::max());
  cv::Mat& ret(result->newMat(original));
  std::vector<math::OuterBorder> outer_edges;
  std::vector<math::InnerBorder> inner_edges;
  std::vector<std::vector<cv::Point>> edges;

  // Step 1
  math::findConnectedComponentEdges(erroded, edges, area);
  qDebug() << "Found" << edges.size() << "contours";

  // Step 2
  for (std::size_t i = 0; i < edges.size(); ++i) {
    auto edge = edges[i];
    /*std::vector<cv::Point> curve;
    double				   eps = 0.05 *
    cv::arcLength(contours[i], true);
    // good? cv::approxPolyDP(contours[i], curve, eps, true);*/
    double a = cv::contourArea(edge);

    if (area.excludes(a)) continue;
    // Filter out all curves with 4 or 6 points and size atleast 1/16 or the
    // image size
    if ((edge.size() == 6 || edge.size() == 4)) {
      if (edge.size() == 6) {
        math::InnerBorder border;
        std::copy(edge.begin(), edge.begin() + 6, border.begin());
        inner_edges.push_back(border);
      } else {
        math::OuterBorder border;
        std::copy(edge.begin(), edge.begin() + 4, border.begin());
        outer_edges.push_back(border);
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
  math::Range<double> outer_side_ratio(optimal - eps, optimal + eps);

  // Step 3
  for (auto it = outer_edges.begin(); it != outer_edges.end();) {
    auto polygon(*it);

    double w =
        math::distance(polygon[0].x, polygon[0].y, polygon[1].x, polygon[1].y);
    double h = math::distance(polygon[1].x, polygon[1].y, polygon[2].x,
                              polygon[2].y);  // TODO use cv::norm

    double r(std::min(w, h) / double(std::max(w, h)));
    if (!outer_side_ratio.contains(r))
      outer_edges.erase(it);
    else
      ++it;
  }

  if (!outer_edges.size())
    throw std::runtime_error("Could not approximate outer edges");

  // Step 4: Filter 6p polygons
  // Distances between the 4p's and 6p's to find the two, that are closest
  // together and where the 4p strictly includes the 6p first int is the
  // inner_edge index second one the outer
  std::vector<std::tuple<std::size_t /*inner*/, std::size_t /*outer*/, double>>
      corner_distances;
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

      // Step 5
      for (std::size_t c = 0; c < outer.size(); ++c) {
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

  // Step 6
  // Now filter for the best match by the Sum of Squared Error
  auto it = std::min_element(
      corner_distances.begin(), corner_distances.end(),
      [](std::tuple<std::size_t, std::size_t, double> const& a,
         std::tuple<std::size_t, std::size_t, double> const& b) {
        return std::get<2>(a) < std::get<2>(b);
      });
  if (it == corner_distances.end())
    throw std::runtime_error("Could detect rotation of inner edges");

  int i = std::get<0>(*it), j = std::get<1>(*it);
  auto inner_edge = inner_edges[std::get<0>(*it)];
  auto outer_edge = outer_edges[std::get<1>(*it)];
  double best_dist = std::get<2>(*it);

  // Step 7
  result->original_ = Plate(outer_edge, inner_edge);
  result->rotated_ = result->original_.normalized(ret, ret);
}
}  // namespace c3picko
