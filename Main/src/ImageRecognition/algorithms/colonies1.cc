#include "ImageRecognition/algorithms/colonies1.h"
#include "ImageRecognition/algo_setting.h"
#include "ImageRecognition/algorithm_job.h"
#include "ImageRecognition/algorithm_result.h"
#include "ImageRecognition/algorithms/helper.h"
#include "ImageRecognition/colony.h"
#include "ImageRecognition/plates/plate.h"
#include <QFuture>
#include <QFutureSynchronizer>
#include <QtConcurrent>
#include <functional>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

namespace c3picko
{
namespace algorithms
{
	Colonies1::Colonies1()
		: Algorithm(
			  "1", "Normal1", "Detects colonies with standard illumination",
			  {(AlgoStep)Colonies1::cvt, (AlgoStep)Colonies1::mask, (AlgoStep)Colonies1::threshold, (AlgoStep)Colonies1::erodeAndDilate,
			   (AlgoStep)Colonies1::label, (AlgoStep)Colonies1::safetyMargin /*,
   (AlgoStep)&Normal1::relativeFiltering*/},
			  {AlgoSetting::make_rangeslider_double("area", "Area", "lel", 50, 2000, 1, {120, 1000}),
			   AlgoSetting::make_rangeslider_double("safety_margin_lr", "Safety Margin Left-Right", "", 0, 1, .01, {.1, .9}, Qt::gray),
			   AlgoSetting::make_rangeslider_double("safety_margin_tb", "Safety Margin Top-Bot", "", 0, 1, .01, {.1, .9}, Qt::gray),
			   AlgoSetting::make_rangeslider_double("aabb_ratio", "AABB Side Ratio", "", 0, 1, .001, {.7, 1}, Qt::cyan),
			   AlgoSetting::make_rangeslider_double("bb_ratio", "BB Side Ratio", "", 0, 1, .001, {.7, 1}, Qt::magenta),
			   AlgoSetting::make_rangeslider_double("convexity", "Convexity", "", 0, 1, .001, {.8, 1}, Qt::green),
			   AlgoSetting::make_rangeslider_double("circularity", "Circularity", "", 0, 1, .001, {.6, 1}, Qt::blue),
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
	static AlgoSetting::ID roundness(int area, int w, int h, std::vector<cv::Point> const& contour, math::Range<double> aabb_ratio,
									 math::Range<double> bb_ratio, math::Range<double> circularity, math::Range<double> convexity)
	{
		double ratio = (std::min(w, h) / double(std::max(w, h)));

		if (aabb_ratio.excludes(ratio))
			return "aabb_ratio";

		// TODO try watershed algorithm

		cv::RotatedRect rrect = cv::minAreaRect(contour);

		ratio = (std::min(rrect.size.width, rrect.size.height) / double(std::max(rrect.size.width, rrect.size.height)));
		if (bb_ratio.excludes(ratio))
			return "bb_ratio";

		double circ((4 * M_PI * area) / std::pow(cv::arcLength(contour, true), 2));
		if (circularity.excludes(circ)) // flouro .8
			return "circularity";
		std::vector<cv::Point> hull, hull_contour;

		cv::convexHull(contour, hull, false, false);
		cv::approxPolyDP(hull, hull_contour, .001, true);

		double convex(cv::contourArea(contour) / cv::contourArea(hull_contour));

		return convexity.contains(convex) ? "" : "convexity"; // flouro .91
	}

	void Colonies1::cvt(AlgorithmJob* base, DetectionResult* result)
	{
		cv::Mat& input  = *reinterpret_cast<cv::Mat*>(base->input().front());
		cv::Mat& output = result->newMat();

		cv::cvtColor(input, output, CV_BGR2GRAY);
	}

	void Colonies1::mask(AlgorithmJob* base, DetectionResult* result)
	{
		if (base->input().size() < 2)
		{
			qWarning("Colony detection missing plate, skipping mask");
			return;
		}

		cv::Mat& input  = result->oldMat();
		cv::Mat& output = result->newMat();

		Plate const* plate(static_cast<Plate*>(base->input()[1]));
		plate->mask(input, output);
	}

	void Colonies1::threshold(AlgorithmJob*, DetectionResult* result)
	{
		// Calculate mean and standart deviation
		cv::Mat& input  = result->oldMat();
		cv::Mat& output = result->newMat();

		cv::adaptiveThreshold(input, output, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 51, 1);
	}

	void Colonies1::erodeAndDilate(AlgorithmJob*, DetectionResult* result)
	{
		cv::Mat& input  = result->oldMat();
		cv::Mat& output = result->newMat();

		int		erosion_type = cv::MORPH_ELLIPSE;
		int		erosion_size = 2;
		cv::Mat kernel		 = getStructuringElement(erosion_type, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
												 cv::Point(erosion_size, erosion_size));

		cv::erode(input, output, kernel);
		cv::dilate(output, output, kernel);
	}

	void Colonies1::label(AlgorithmJob* base, DetectionResult* result)
	{
		cv::Mat&			 input	= result->oldMat();
		std::vector<Colony>& colonies = result->colonies();

		// std::atomic<Colony::ID> id(-1);
		math::Range<double> _area		  = base->settingById("area").value<math::Range<double>>();
		math::Range<double> _aabb_ratio   = base->settingById("aabb_ratio").value<math::Range<double>>();
		math::Range<double> _bb_ratio	 = base->settingById("bb_ratio").value<math::Range<double>>();
		math::Range<double> _circularity  = base->settingById("circularity").value<math::Range<double>>();
		math::Range<double> _convexity	= base->settingById("convexity").value<math::Range<double>>();
		bool				show_excluded = false; // base->settingById("show_excluded_by_algo").value<bool>();

		cv::Mat stats, labeled, centers;

		// Label every connected component
		labeled = cv::Mat(input.rows, input.cols, CV_32S);
		cv::connectedComponentsWithStats(input, labeled, stats, centers);
		colonies.resize(centers.rows, Colony(0, 0, 0, 0, 0, 0, 0, 0));

		// This lambda expression will be executed for every detected component.
		// It needs to be a function, because we want to thread it .
		std::function<void(int, int)> map = [&centers, &colonies, &labeled, &stats, base, &input, &show_excluded, &_aabb_ratio, &_area,
											 &_bb_ratio, &_circularity, &_convexity](int start, int count) -> void {
			Colony::ID   id		 = start;
			const double scale_x = 1.0 / input.cols, scale_y = 1.0 / input.rows;

			for (int i = 0; i < count; ++i)
			{
				int  colony_index = start + i;
				auto center		  = cv::Point2d(centers.at<cv::Point2d>(colony_index, 0));

				int r = int(center.y), c = int(center.x);
				int label = labeled.at<int>(r, c);

				if (!label)
				{
					if (show_excluded)
						colonies[colony_index]
							= Colony(center.x * scale_x, center.y * scale_y, 0, 0, 10, 0, id++, "show_excluded_by_algo");
					continue;
				}

				int area = stats.at<int>(label, cv::CC_STAT_AREA);
				int w	= stats.at<int>(label, cv::CC_STAT_WIDTH);
				int h	= stats.at<int>(label, cv::CC_STAT_HEIGHT);
				int top  = stats.at<int>(label, cv::CC_STAT_TOP);
				int left = stats.at<int>(label, cv::CC_STAT_LEFT);

				if (!_area.contains(area))
				{
					colonies[colony_index]
						= Colony(center.x * scale_x, center.y * scale_y, area, 0, std::sqrt(area * M_1_PI), 0, id++, "area");
					continue;
				}
				// Filter by roundness
				else
				{
					std::vector<std::vector<cv::Point>> contours;

					// We check the AABB_RATIO twice for speed NOTE look into this in
					// benchmark phase
					if (_aabb_ratio.excludes(std::min(w, h) / double(std::max(w, h))))
					{
						colonies[colony_index]
							= Colony(center.x * scale_x, center.y * scale_y, area, 0, std::sqrt(area * M_1_PI), 0, id++, "aabb_ratio");
					}

					cv::Mat submat(input, cv::Rect(left, top, w, h));
					cv::findContours(submat, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

					AlgoSetting::ID excluded_by = roundness(area, w, h, contours[0], _aabb_ratio, _bb_ratio, _circularity, _convexity);

					if (!excluded_by.isEmpty())
						colonies[colony_index]
							= Colony(center.x * scale_x, center.y * scale_y, area, 0, std::sqrt(area * M_1_PI), 0, id++, excluded_by);
					else
					{
						double circ = cv::arcLength(contours[0], true);

						// NOTE Brightness only works on unmasked original image contours,
						// otherwise they need conversion
						double br = 0; /*math::brightness(
			  contours[0], *reinterpret_cast<cv::Mat*>(base->input().front()));*/
						colonies[colony_index] = Colony(center.x * scale_x, center.y * scale_y, area, circ, circ * M_1_PI_2, br, id++, "");
					}
				}
			}
		};

		// Parallel part
		const int threads   = base->pool()->maxThreadCount();
		const int task_size = centers.rows / threads;
		Q_ASSERT(threads && task_size);
		QFutureSynchronizer<void> barrier; // synchronises in dtor

		for (int i = 0; i < threads; ++i)
		{
			int start = task_size * i;
			int count = (i == threads - 1) ? (centers.rows - start) : task_size;

			auto future = QtConcurrent::run(base->pool(), map, start, count);
			barrier.addFuture(future);
		}

		// NOTE QtConcurrent::blockingMappedReduced could also be used, but has the
		// disadvantage that the ThreadPool cant be specified .
		// Mixing std's parallelism in here would be even worse, better stay with one
		// threading library .
	}

	// TODO round safety margin
	void Colonies1::safetyMargin(AlgorithmJob* base, DetectionResult* result)
	{
		std::vector<Colony>& input	 = result->colonies();
		auto				 margin_lr = base->settingById("safety_margin_lr").value<math::Range<double>>();
		auto				 margin_tb = base->settingById("safety_margin_tb").value<math::Range<double>>();
		cv::Rect2d			 bounding(cv::Point2d(margin_lr.lower_, margin_tb.lower_), cv::Point2d(margin_lr.upper_, margin_tb.upper_));

		for (auto it = input.begin(); it != input.end(); ++it)
		{
			Colony const& c = *it;
			cv::Point2d   cp(c.x(), c.y());

			if (!c.included())
				continue;
			if (!bounding.contains(cp))
				it->setExcluded_by("safety_margin_lt");
		}
	}

	void Colonies1::relativeFiltering(AlgorithmJob* base, DetectionResult* result)
	{
		std::vector<Colony>* input = &result->colonies();

		// Should we skip the step?
		if (!base->settingById("filter_relative").value<bool>())
			return;
		QString option = base->settingById("relative_filter").option<QString>().toLower();

		math::Range<double> _rel = base->settingById("rel").value<math::Range<double>>();

		cv::Scalar				mean, stddev;
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
		double meand   = cv::sum(mean)[0];
		double stddefd = cv::sum(stddev)[0];

		double rel_min = meand + _rel.lower_ * stddefd; // TODO
		double rel_max = meand + _rel.upper_ * stddefd;

		for (auto it = input->begin(); it != input->end();)
		{
			if ((*(it).*get_data)() < rel_min || (*(it).*get_data)() > rel_max)
				input->erase(it);
			else
				++it;
		}
	}
} // namespace algorithms
} // namespace c3picko
