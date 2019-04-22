#include "ImageRecognition/algorithms/fluro1.h"
#include <opencv2/opencv.hpp>
#include "ImageRecognition/algorithm_result.h"
#include "ImageRecognition/algorithms/normal1.h"

namespace c3picko {
Fluro1::Fluro1()
    : Algorithm(
          "2", "Fluro1", "Detects fluorescente colonies",
          {(AlgoStep)&Normal1::cvt, (AlgoStep)&Fluro1::threshold,
           (AlgoStep)&Normal1::erodeAndDilate, (AlgoStep)&Normal1::label /*,
		   (AlgoStep)&Normal1::relativeFiltering*/},
          {/*AlgoSetting::make_checkbox("show_excluded_by_algo",
                                                                  "Show ignored
              by algorithm", "", true, {}, Qt::red),*/
           AlgoSetting::make_rangeslider_double("area", "Area", "lel", 10, 2000,
                                                1, {120, 1000}),
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
          true, 3000) {}

void Fluro1::threshold(AlgorithmJob *base, DetectionResult *result) {
  cv::Mat &input = result->oldMat();
  cv::Mat &output = result->newMat();

  cv::Scalar mean, stddev;
  // Filter by brightest pixels
  cv::meanStdDev(input, mean, stddev);
  cv::threshold(input, output, mean[0] + 1.5 * stddev[0], 255,
                cv::THRESH_BINARY);  // flour TODO tryp Otsu's method
}
}  // namespace c3picko
