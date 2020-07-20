#include "ImageRecognition/algorithms/plate_round.h"

#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

#include "ImageRecognition/algorithm_job.h"
#include "ImageRecognition/algorithms/colonies1.h"
#include "ImageRecognition/algorithms/helper.h"
#include "ImageRecognition/algorithms/plate_rect.h"
#include "ImageRecognition/plate_result.h"
#include "ImageRecognition/plates/round_plate.h"

namespace c3picko {
namespace algorithms {
PlateRound::PlateRound()
	: Algorithm("kROUND", "Round1",
				"Detects a round plate with red frame attached",
				{(AlgoStep)Colonies1::cvt, (AlgoStep)PlateRect::threshold,
				 (AlgoStep)PlateRound::detect},
				{}, true, 5000) {}

void PlateRound::detect(AlgorithmJob* base, PlateResult* result) {
  /**
   * Steps:
   *
   * 1. Detect the contours of all connected components in the black and white
   *    input image. Approximate the contours with polygons, now called edge.
   *    (same as plate1.cc)
   * 2.
   */

  cv::Mat& input = result->oldMat();
  cv::Mat& output =
	  result->newMat(*static_cast<cv::Mat*>(base->input().front()));

  int pxls = input.cols * input.rows;
  math::Range<int> area{pxls / 16, pxls};
  std::vector<std::vector<cv::Point>> edges, raw_edges;
  raw_edges = math::findConnectedComponentEdges(input, edges, area, .0025);

  // FIXME this assumes, that only the two correct contours got detected - for
  // debugging index of the inner border
  if (edges.size() != 2) throw Exception("Did not detect exactly two contours");
  std::size_t inner = 0, outer = 1;

  if (!math::polyIncludesPoly(edges[outer], edges[inner]))
	std::swap(inner, outer);

  // The inner contour must be the circle
  cv::RotatedRect inner_e = cv::fitEllipseAMS(edges[inner]);
  cv::RotatedRect outer_e = cv::fitEllipseAMS(edges[outer]);
  cv::Point center = inner_e.center;

  // calculate approximation errors, assuming the contours as a circle frees us
  // from implementing a point-to-ellipse distance function.
  {
	double inner_err = qAbs(cv::contourArea(edges[inner]) -
							(M_PI * inner_e.size.width * inner_e.size.height)) /
					   cv::contourArea(edges[inner]);
	double outer_err = qAbs(cv::contourArea(edges[outer]) -
							(M_PI * outer_e.size.width * outer_e.size.height)) /
					   cv::contourArea(edges[outer]);

	if (inner_err >= outer_err)
	  throw Exception("Outer edge was rounder than inner edge");
  }

  double inner_r =
	  (inner_e.size.width + inner_e.size.height) / 4;  // /4 b its the diameter
  double outer_r = (inner_r / 82.9) * 91.1;	 // FIXME get values from profile
  double margin_r = 1.03;
  // Calculate markers
  std::array<cv::Point, 3> markers;
  {
	std::vector<cv::Point2f> outer_points;	// kmeans only works with floats
	std::vector<std::vector<cv::Point>> marker_data(3);
	std::vector<int> labels;
	// Cluster points of the outer contour
	for (std::size_t i = 0; i < raw_edges[outer].size(); ++i) {
	  cv::Point v(raw_edges[outer][i] - center);
	  double d = math::norm_l2(0, 0, v.x, v.y);

	  if (d > outer_r * margin_r) outer_points.push_back(raw_edges[outer][i]);
	}

	cv::kmeans(outer_points, 3, labels,
			   cv::TermCriteria(cv::TermCriteria::COUNT, 1, 10), 1,
			   cv::KMEANS_PP_CENTERS);
	if (labels.empty() || *std::max_element(labels.begin(), labels.end()) != 2)
	  throw Exception("Cant find all three markers");

	// TODO instead of the gravitational center we can also try to approximate
	// the marker_data by three triangles and take the interserction point of
	// the two smallest edges
	for (std::size_t i = 0; i < outer_points.size(); ++i)
	  marker_data[labels[i]].push_back(outer_points[i]);

	for (std::size_t i = 0; i < markers.size(); ++i) {
	  std::vector<cv::Point> hull;
	  cv::convexHull(marker_data[i], hull);
	  markers[i] = math::gravityCenter(hull);
	}
  }

  // Create the plates
  std::unique_ptr<RoundPlate> unrotated(
	  std::make_unique<RoundPlate>(edges[outer], edges[inner], markers));

  // Draw the inner ellipse and markers (optional)
  {
	cv::ellipse(output, inner_e, cv::Scalar::all(255), 2, cv::LINE_AA);
	cv::drawMarker(output, markers[unrotated->m1()], cv::Scalar(0, 255, 0),
				   cv::MARKER_CROSS, 30, 2);
	cv::drawMarker(output, markers[(unrotated->m1() + 1) % 3],
				   cv::Scalar(255, 255, 0), cv::MARKER_TILTED_CROSS, 30, 2);
	cv::drawMarker(output, markers[(unrotated->m1() + 2) % 3],
				   cv::Scalar(255, 255, 0), cv::MARKER_TILTED_CROSS, 30, 2);
  }

  // Rotate/Cut the final image
  unrotated->crop(output, output);

  // Set result output
  result->original() = std::move(unrotated);
}
}  // namespace algorithms
}  // namespace c3picko
