/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "ImageRecognition/algo_setting.h"
#include "ImageRecognition/algorithm_job.h"
#include "ImageRecognition/algorithm_result.h"
#include "ImageRecognition/algorithms/helper.h"
#include "ImageRecognition/algorithms/normal1.h"
#include "ImageRecognition/colony.h"
#include "ImageRecognition/plates/plate.h"

namespace d3picko {
Normal1::Normal1()
	: Algorithm(
		  "1", "Normal1", "Detects colonies with standard illumination",
		  {(AlgoStep)Normal1::cvt, (AlgoStep)Normal1::mask,
		   (AlgoStep)Normal1::threshold, (AlgoStep)Normal1::erodeAndDilate,
		   (AlgoStep)Normal1::label, (AlgoStep)Normal1::safetyMargin /*,
		   (AlgoStep)&Normal1::relativeFiltering*/},
		  {AlgoSetting::make_rangeslider_double("area", "Area", "lel", 50, 2000,
												1, {120, 1000}),
		   AlgoSetting::make_rangeslider_double("safety_margin_lr",
												"Safety Margin Left-Right", "", 0,
												1, .01, {.1, .9}, Qt::gray),
		   AlgoSetting::make_rangeslider_double("safety_margin_tb",
												"Safety Margin Top-Bot", "", 0,
												1, .01, {.1, .9}, Qt::gray),
		   AlgoSetting::make_rangeslider_double("aabb_ratio", "AABB Side Ratio",
												"", 0, 1, .001, {.7, 1},
												Qt::cyan),
		   AlgoSetting::make_rangeslider_double("bb_ratio", "BB Side Ratio", "",
												0, 1, .001, {.7, 1},
												Qt::magenta),
		   AlgoSetting::make_rangeslider_double("convexity", "Convexity", "", 0,
												1, .001, {.8, 1}, Qt::green),
		   AlgoSetting::make_rangeslider_double(
			   "circularity", "Circularity", "", 0, 1, .001, {.6, 1}, Qt::blue),
		   //	 AlgoSetting::make_checkbox("plate_detection", "Detect the
		   // plate", "", true),
		   /*AlgoSetting::make_checkbox(
			   "filter_relative", "Filter detected colonies", "", false,
			   {AlgoSetting::make_dropdown("relative_filter", "Filter Colonies",
										   "Select an attribute to filter",
										   {{"n", "None"},
											{"a", "Area"},
											{"r", "Circumference"},
											{"b", "brightness"}},
										   "n"),
				AlgoSetting::make_rangeslider_double(
					"rel", "Mean", "Mean +x*Standard deviation", -5, 5, .01,
					{-5, 5})},
			   Qt::red)*/},
		  true, 3000)
{
}

/**
 * @brief Checks the roundness of the given contour.
 * @return ID of a setting that leaded to the exclusion of the colony
 * or empty.
 */
static AlgoSetting::ID roundness(int area, int w, int h,
								 std::vector<cv::Point> const& contour,
								 math::Range<double> aabb_ratio,
								 math::Range<double> bb_ratio,
								 math::Range<double> circularity,
								 math::Range<double> convexity) {
  double ratio = (std::min(w, h) / double(std::max(w, h)));

  if (aabb_ratio.excludes(ratio)) return "aabb_ratio";

  // TODO try watershed algorithm

  cv::RotatedRect rrect = cv::minAreaRect(contour);

  ratio = (std::min(rrect.size.width, rrect.size.height) /
		   double(std::max(rrect.size.width, rrect.size.height)));
  if (bb_ratio.excludes(ratio)) return "bb_ratio";

  double circ((4 * M_PI * area) / std::pow(cv::arcLength(contour, true), 2));
  if (circularity.excludes(circ))  // flouro .8
	return "circularity";
  std::vector<cv::Point> hull, hull_contour;

  cv::convexHull(contour, hull, false, false);
  cv::approxPolyDP(hull, hull_contour, .001, true);

  double convex(cv::contourArea(contour) / cv::contourArea(hull_contour));

  return convexity.contains(convex) ? "" : "convexity";	 // flouro .91
}

void Normal1::cvt(AlgorithmJob* base, DetectionResult* result) {
  cv::Mat& input = *reinterpret_cast<cv::Mat*>(base->input().front());
  cv::Mat& output = result->newMat();

  cv::cvtColor(input, output, CV_BGR2GRAY);
}

void Normal1::mask(AlgorithmJob* base, DetectionResult* result) {
  if (base->input().size() < 2) {
	qWarning("Colony detection missing plate, skipping crop");
	return;
  }

  cv::Mat& input = result->oldMat();
  cv::Mat& output = result->newMat();

  Plate const* plate((Plate*)base->input()[2]);
  plate->mask(input, output);
}

void Normal1::threshold(AlgorithmJob*, DetectionResult* result) {
  // Calculate mean and standart deviation
  cv::Mat& input = result->oldMat();
  cv::Mat& output = result->newMat();

  cv::Scalar mean, stddev;
  cv::adaptiveThreshold(input, output, 255, cv::ADAPTIVE_THRESH_MEAN_C,
						cv::THRESH_BINARY_INV, 51,
						1);	 // non flour
}

void Normal1::erodeAndDilate(AlgorithmJob*, DetectionResult* result) {
  cv::Mat& input = result->oldMat();
  cv::Mat& output = result->newMat();

  int erosion_type = cv::MORPH_ELLIPSE;
  int erosion_size = 2;
  cv::Mat kernel = getStructuringElement(
	  erosion_type, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
	  cv::Point(erosion_size, erosion_size));

  cv::erode(input, output, kernel);
  cv::dilate(output, output, kernel);
}

void Normal1::label(AlgorithmJob* base, DetectionResult* result) {
  cv::Mat& input = result->oldMat();
  std::vector<Colony>& colonies = result->colonies();

  Colony::ID id = 0;
  math::Range<double> _area =
	  base->settingById("area").value<math::Range<double>>();
  math::Range<double> _aabb_ratio =
	  base->settingById("aabb_ratio").value<math::Range<double>>();
  math::Range<double> _bb_ratio =
	  base->settingById("bb_ratio").value<math::Range<double>>();
  math::Range<double> _circularity =
	  base->settingById("circularity").value<math::Range<double>>();
  math::Range<double> _convexity =
	  base->settingById("convexity").value<math::Range<double>>();
  bool show_excluded =
	  false;  // base->settingById("show_excluded_by_algo").value<bool>();

  cv::Mat stats, labeled, centers;

  // Label every connected component
  labeled = cv::Mat(input.rows, input.cols, CV_32S);
  cv::connectedComponentsWithStats(input, labeled, stats, centers);

  for (int i = 0; i < centers.rows; ++i) {
	// In this much quicker approach we assume, that the center always lies
	// ontop of a pixel of the component.
	// Should be no problem with convex colonies.
	auto center =
		cv::Point2d(centers.at<double>(i, 0), centers.at<double>(i, 1));
	int r = int(center.y), c = int(center.x);
	int label = labeled.at<int>(r, c);

	if (!label) {
	  if (show_excluded)
		colonies.emplace_back(center.x / double(input.cols),
							  center.y / double(input.rows), 0, 0, 10, 0, id++,
							  "show_excluded_by_algo");
	  continue;
	}

	int area = stats.at<int>(label, cv::CC_STAT_AREA);
	int w = stats.at<int>(label, cv::CC_STAT_WIDTH);
	int h = stats.at<int>(label, cv::CC_STAT_HEIGHT);
	int top = stats.at<int>(label, cv::CC_STAT_TOP);
	int left = stats.at<int>(label, cv::CC_STAT_LEFT);

	if (!_area.contains(area)) {
	  colonies.emplace_back(center.x / double(input.cols),
							center.y / double(input.rows), area, 0,
							std::sqrt(area / M_PI), 0, id++, "area");
	  continue;
	}
	// Filter by roundness
	else {
	  std::vector<std::vector<cv::Point>> contours;

	  // We check the AABB_RATIO twice for speed NOTE look into this in
	  // benchmark phase
	  if (_aabb_ratio.excludes(std::min(w, h) / double(std::max(w, h)))) {
		colonies.emplace_back(
			Colony(center.x / double(input.cols), center.y / double(input.rows),
				   area, 0, std::sqrt(area / M_PI), 0, id++, "aabb_ratio"));
	  }

	  cv::Mat submat(input, cv::Rect(left, top, w, h));
	  cv::findContours(submat, contours, cv::RETR_EXTERNAL,
					   cv::CHAIN_APPROX_SIMPLE);

	  AlgoSetting::ID excluded_by =
		  roundness(area, w, h, contours[0], _aabb_ratio, _bb_ratio,
					_circularity, _convexity);

	  if (!excluded_by.isEmpty())
		colonies.emplace_back(
			Colony(center.x / double(input.cols), center.y / double(input.rows),
				   area, 0, std::sqrt(area / M_PI), 0, id++, excluded_by));
	  else {
		double circ = cv::arcLength(contours[0], true);

		// NOTE Brightness only works on unmasked original image contours,
		// otherwise they need conversion
		double br = math::brightness(
			contours[0], *reinterpret_cast<cv::Mat*>(base->input().front()));
		colonies.emplace_back(center.x / double(input.cols),
							  center.y / double(input.rows), area, circ,
							  circ / (2 * M_PI), br, id++, "");
	  }
	}
  }
}

void Normal1::safetyMargin(AlgorithmJob* base, DetectionResult* result) {
  std::vector<Colony>& input = result->colonies();
  auto margin_lr =
	  base->settingById("safety_margin_lr").value<math::Range<double>>();
  auto margin_tb =
	  base->settingById("safety_margin_tb").value<math::Range<double>>();
  cv::Rect2d bounding(cv::Point2d(margin_lr.lower_, margin_tb.lower_),
					  cv::Point2d(margin_lr.upper_, margin_tb.upper_));

  for (auto it = input.begin(); it != input.end(); ++it) {
	Colony const& c = *it;
	cv::Point2d cp(c.x(), c.y());

	if (!c.included()) continue;
	if (!bounding.contains(cp)) it->setExcluded_by("safety_margin_lt");
  }
}

void Normal1::relativeFiltering(AlgorithmJob* base, DetectionResult* result) {
  std::vector<Colony>* input = &result->colonies();

  // Should we skip the step?
  if (!base->settingById("filter_relative").value<bool>()) return;
  QString option =
	  base->settingById("relative_filter").option<QString>().toLower();

  math::Range<double> _rel =
	  base->settingById("rel").value<math::Range<double>>();

  cv::Scalar mean, stddev;
  std::vector<cv::Scalar> values(input->size(), 0);

  double (Colony::*get_data)(void) const = nullptr;

  if (option == "area")
	get_data = &Colony::area;
  else if (option == "circumference")
	get_data = &Colony::circumference;
  else if (option == "brightness")
	get_data = &Colony::brightness;
  else
	throw Exception("Unknown dropdown option " + option);

  for (std::size_t i = 0; i < input->size(); ++i)
	values[i] = (input->at(i).*get_data)();

  cv::meanStdDev(values, mean, stddev);
  double meand = cv::sum(mean)[0];
  double stddefd = cv::sum(stddev)[0];

  double rel_min = meand + _rel.lower_ * stddefd;  // TODO
  double rel_max = meand + _rel.upper_ * stddefd;

  for (auto it = input->begin(); it != input->end();) {
	if ((*(it).*get_data)() < rel_min || (*(it).*get_data)() > rel_max)
	  input->erase(it);
	else
	  ++it;
  }
}
}  // namespace d3picko
