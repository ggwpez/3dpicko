#include "include/algorithms/algo1_test.h"
#include "include/algorithm_result.h"
#include "include/algorithms/helper.h"
#include "include/colony.hpp"
#include "include/colony_type.h"
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

namespace c3picko
{

Algo1Test::Algo1Test()
	: Algorithm("1", "Fluro1", "", {&Algo1Test::cvt, &Algo1Test::threshold, &Algo1Test::erodeAndDilate, &Algo1Test::plateDetection,
									&Algo1Test::label, &Algo1Test::relativeFiltering},
				{AlgoSetting::make_checkbox("plate_detection", "Detect the plate", "", true),
				 AlgoSetting::make_checkbox("fluro_thresh", "Threshold by brightness", "", false),
				 AlgoSetting::make_slider_int("area_max", "Max Radius", "lel", 10, 2000, 1, 500),
				 AlgoSetting::make_slider_int("area_min", "Min Radius", "lel", 10, 2000, 1, 100),
				 AlgoSetting::make_dropdown("relative_filter", "Filter Colonies", "Select an attribute to filter",
											{{"n", "None"}, {"a", "Area"}, {"r", "Circumference"}, {"b", "brightness"}}, "n"),
				 AlgoSetting::make_slider_double("rel_max", "Max Mean", "Mean +x*Standard deviation", -5, 5, .01, 5),
				 AlgoSetting::make_slider_double("rel_min", "Min Mean", "Mean +x*Standard deviation", -5, 5, .01, -5),
				 AlgoSetting::make_slider_double("aabb_ratio_min", "AABB Side Ratio max", "", 0, 1, .001, .7),
				 AlgoSetting::make_slider_double("aabb_ratio_max", "AABB Side Ratio min", "", 0, 1, .001, 1),
				 AlgoSetting::make_slider_double("bb_ratio_min", "BB Side Ratio min", "", 0, 1, .001, .7),
				 AlgoSetting::make_slider_double("bb_ratio_max", "BB Side Ratio max", "", 0, 1, .001, 1),
				 AlgoSetting::make_slider_double("convexity_min", "Convexitx min", "", 0, 1, .001, .8),
				 AlgoSetting::make_slider_double("convexity_max", "Convexitx max", "", 0, 1, .001, 1),
				 AlgoSetting::make_slider_double("circularity_min", "Circularity min", "", 0, 1, .001, .6),
				 AlgoSetting::make_slider_double("circularity_max", "Circularity max", "", 0, 1, .001, 1)},
				true)
{
}

Algo1Test::~Algo1Test() {}

void Algo1Test::render_rrect(cv::Mat& out, cv::RotatedRect rect)
{
	cv::Point2f vertices2f[4];
	rect.points(vertices2f);

	cv::Point vertices[4];
	for (int i		= 0; i < 4; ++i)
		vertices[i] = vertices2f[i];
	cv::fillConvexPoly(out, vertices, 4, cv::Vec3b(255, 255, 255));
}

void Algo1Test::drawText(cv::Mat& img, cv::Mat& output, std::vector<cv::Vec3f>& colonies)
{
	cv::Point2i img_center{img.cols / 2, img.rows / 2};

	// Sort the colonies by ascending distance from the center
	std::sort(colonies.begin(), colonies.end(), [&img_center](cv::Vec3f const& a, cv::Vec3f const& b) {
		return (std::pow(img_center.x - a[0], 2) + std::pow(img_center.y - a[1], 2))
			   < (std::pow(img_center.x - b[0], 2) + std::pow(img_center.y - b[1], 2));
	});

	int f_type  = cv::FONT_HERSHEY_SIMPLEX;
	int f_scale = 2;
	int f_thick = 2;

	for (int i = 0; i < colonies.size(); ++i)
	{
		std::string text = std::to_string(i);
		cv::Point   pos{int(colonies[i][0]), int(colonies[i][1])};

		cv::Size  text_size = cv::getTextSize(text, f_type, f_scale, f_thick, nullptr);
		cv::Point t_pos		= {pos.x - text_size.width / 2, pos.y + text_size.height / 2};
		cv::putText(output, text, t_pos, f_type, f_scale, cv::Scalar::all(255), f_thick);
	}
}

static bool roundness(int area, int w, int h, std::vector<cv::Point> const& contour, math::Range<double> aabb_ratio,
					  math::Range<double> bb_ratio, math::Range<double> circularity, math::Range<double> convexity)
{
	double ratio = (std::min(w, h) / double(std::max(w, h)));

	if (aabb_ratio.excludes(ratio))
		return false;

	// TODO try watershed algorithm

	cv::RotatedRect rrect = cv::minAreaRect(contour);

	ratio = (std::min(rrect.size.width, rrect.size.height) / double(std::max(rrect.size.width, rrect.size.height)));
	if (bb_ratio.excludes(ratio))
		return false;

	double circ((4 * M_PI * area) / std::pow(cv::arcLength(contour, true), 2));
	if (circularity.excludes(circ)) // flouro .8
	{
		return false;
	}
	std::vector<cv::Point> hull, hull_contour;

	cv::convexHull(contour, hull, false, false);
	cv::approxPolyDP(hull, hull_contour, .001, true);

	double convex(cv::contourArea(contour) / cv::contourArea(hull_contour));

	return convexity.contains(convex); // flouro .91
}

void Algo1Test::cvt(Algorithm* base, AlgorithmResult* result)
{
	result->newMat();

	cv::Mat* input = reinterpret_cast<cv::Mat*>(base->input());
	cv::cvtColor(*input, result->newMat(), CV_BGR2GRAY);
}

void Algo1Test::threshold(Algorithm* base, AlgorithmResult* result)
{
	bool fluro_thresh = base->settingById("fluro_thresh").value<bool>();
	// Calculate mean and standart deviation
	cv::Mat& input  = result->oldMat();
	cv::Mat& output = result->newMat();

	cv::Scalar mean, stddev;
	// Filter by brightest pixels
	if (fluro_thresh)
	{
		cv::meanStdDev(input, mean, stddev);
		cv::threshold(input, output, mean[0] + 1.5 * stddev[0], 255, cv::THRESH_BINARY); // flour TODO tryp Otsu's method
	}
	else
		cv::adaptiveThreshold(input, output, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 51, 1); // non flour
}

void Algo1Test::erodeAndDilate(Algorithm* base, AlgorithmResult* result)
{
	cv::Mat& input  = result->oldMat();
	cv::Mat& output = result->newMat();

	int		erosion_type = cv::MORPH_ELLIPSE;
	int		erosion_size = 2;
	cv::Mat kernel
		= getStructuringElement(erosion_type, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), cv::Point(erosion_size, erosion_size));

	cv::erode(input, output, kernel);
	cv::dilate(output, output, kernel);
}

void Algo1Test::plateDetection(Algorithm* base, AlgorithmResult* result)
{
	cv::Mat& erroded = result->oldMat();
	// cv::Mat& output  = result->newMat(*reinterpret_cast<cv::Mat*>(base->input()));
	cv::Mat output(*reinterpret_cast<cv::Mat*>(base->input()));

	if (!base->settingById("plate_detection").value<bool>())
		return;

	cv::Mat								canny_out;
	std::vector<std::vector<cv::Point>> conts;

	cv::Canny(erroded, canny_out, 3, 11);
	cv::findContours(canny_out, conts, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	std::vector<math::LineSeg> lines;
	// Fit lines throug the contours and calculate the angle and length to image coordinate system
	for (std::size_t i = 0; i < conts.size(); ++i)
	{
		cv::RotatedRect rect = cv::minAreaRect(conts[i]);

		double r(std::min(rect.size.width, rect.size.height) / double(std::max(rect.size.width, rect.size.height)));
		if (r > .05 || std::isnan(r) || cv::arcLength(conts[i], false) < 10)
			continue;

		cv::Vec4d fit_line;
		// cv::drawContours(result, conts, i, color, 2, 8);
		cv::fitLine(conts[i], fit_line, CV_DIST_L2, 0, .01, .01);

		double		length = std::sqrt(std::pow(rect.size.width, 2) + std::pow(rect.size.height, 2));
		cv::Point2d v	  = cv::Point2d(fit_line[0] * length / 2, fit_line[1] * length / 2);
		// cv::fitLine matches the lines sometimes form top-to-bottom and sometimes from bottom-to-top
		// so we need to turn the wrong ones around.
		if (fit_line[1] < 0)
		{
			v			= -v;
			fit_line[1] = -fit_line[1];
		}

		cv::Point2d start = cv::Point2d(fit_line[2], fit_line[3]) - v;
		cv::Point2d send  = cv::Point2d(fit_line[2], fit_line[3]) + v;

		math::LineSeg line{start.x, start.y, send.x, send.y, {fit_line[0], fit_line[1]}, length};
		lines.push_back(line);
	}

	std::vector<cv::Scalar>  bin_clrs;
	cv::Mat					 labels;
	std::vector<cv::Point2f> centers;
	// kmeans only works with floats
	std::vector<cv::Point2f> points(lines.size(), cv::Point2f());
	cv::TermCriteria		 crit(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1);

	for (std::size_t i = 0; i < lines.size(); ++i)
	{
		cv::Point2d p(-lines[i].angle.imag(), lines[i].angle.real());
		// std::cout << (std::arg(lines[i].angle) / M_PI) * 180.f << "°\n";
		points[i] = p;
		cv::line(output, cv::Point(output.cols / 2, output.rows / 2),
				 cv::Point2f(output.cols / 2 + p.x * 1000, output.rows / 2 + p.y * 1000), cv::Scalar::all(255), 1);
	}

	int const ks = 3;
	// We need atleast as many points as possible centers
	if (points.size() < ks)
		return;
	cv::kmeans(points, ks, labels, crit, 3, cv::KMEANS_PP_CENTERS, centers);

	for (int i = 0; i < ks; ++i)
		bin_clrs.push_back(cv::Scalar(rand() % 255, rand() % 255, rand() % 255));

	cv::circle(output, cv::Point2f(output.cols / 2, output.rows / 2), 10, cv::Scalar::all(255), 4);
	for (auto const& center : centers)
	{
		cv::line(output, cv::Point2f(output.cols / 2, output.rows / 2),
				 cv::Point2f(output.cols / 2, output.rows / 2) + cv::Point2f(center.x * 60, center.y * 60), cv::Scalar(0, 0, 255), 3);
		// std::cout << "c " << center.x << " / " << center.y << "\n";
	}

	for (int i = 0; i < lines.size(); ++i)
	{
		cv::line(output, cv::Point(lines[i].x, lines[i].y), cv::Point(lines[i].endx, lines[i].endy), bin_clrs[labels.at<int>(i)], 2);
	}
}

void Algo1Test::label(Algorithm* base, AlgorithmResult* result)
{
	cv::Mat& input	= result->oldMat();
	auto&	colonies = result->colonies_;

	Colony::ID			id	= 0;
	math::Range<double> _area = {base->settingById("area_min").value<double>(), base->settingById("area_max").value<double>()};
	math::Range<double> _aabb_ratio
		= {base->settingById("aabb_ratio_min").value<double>(), base->settingById("aabb_ratio_max").value<double>()};
	math::Range<double> _bb_ratio = {base->settingById("bb_ratio_min").value<double>(), base->settingById("bb_ratio_max").value<double>()};
	math::Range<double> _circularity
		= {base->settingById("circularity_min").value<double>(), base->settingById("circularity_max").value<double>()};
	math::Range<double> _convexity
		= {base->settingById("convexity_min").value<double>(), base->settingById("convexity_max").value<double>()};

	cv::Mat stats, labeled, centers;

	// Label every connected component
	labeled = cv::Mat(input.rows, input.cols, CV_32S);
	cv::connectedComponentsWithStats(input, labeled, stats, centers);

	for (int i = 0; i < centers.rows; ++i)
	{
		// In this much quicker approach we assume, that the center always lies
		// ontop of a pixel of the component.
		// Should be no problem with convex colonies.
		auto center = cv::Point2d(centers.at<double>(i, 0), centers.at<double>(i, 1));
		int  r = center.y, c = center.x;
		int  label = labeled.at<int>(r, c);

		if (!label)
			continue;

		int area = stats.at<int>(label, cv::CC_STAT_AREA);
		int w	= stats.at<int>(label, cv::CC_STAT_WIDTH);
		int h	= stats.at<int>(label, cv::CC_STAT_HEIGHT);
		int top  = stats.at<int>(label, cv::CC_STAT_TOP);
		int left = stats.at<int>(label, cv::CC_STAT_LEFT);

		if (!_area.contains(area))
			continue;
		// Filter by roundness
		else
		{
			std::vector<std::vector<cv::Point>> contours;

			if (_aabb_ratio.excludes(std::min(w, h) / double(std::max(w, h))))
				continue;

			cv::Mat submat(input, cv::Rect(left, top, w, h));
			cv::findContours(submat, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

			if (!roundness(area, w, h, contours[0], _aabb_ratio, _bb_ratio, _circularity, _convexity))
				continue;
			else
			{
				double circ = cv::arcLength(contours[0], true);

				// NOTE Brightness only works on unmasked original image contours, otherwise they need conversion
				double br = math::brightness(contours[0], *reinterpret_cast<cv::Mat*>(base->input()));
				Colony detected(center.x / double(input.cols), center.y / double(input.rows), area, circ, circ / (2 * M_PI), br, ++id,
								Colony::Type::INCLUDED);
				colonies.push_back(detected);
			}
		}
	}

	qDebug() << "Detected" << colonies.size() << "colonies";
}

void Algo1Test::relativeFiltering(Algorithm* base, AlgorithmResult* result)
{
	std::vector<Colony>* input = &result->colonies_;
	// Should we skip the step?
	QString option = base->settingById("relative_filter").option<QString>().toLower();
	if (option == "none" || option == "") // Skip or invalid option
		return;

	double rel_min = base->settingById("rel_min").value<double>(), rel_max = base->settingById("rel_max").value<double>();

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
		throw std::runtime_error("Unreachable");

	for (std::size_t i = 0; i < input->size(); ++i)
		values[i]	  = (input->at(i).*get_data)();

	cv::meanStdDev(values, mean, stddev);
	double meand   = cv::sum(mean)[0];
	double stddefd = cv::sum(stddev)[0];

	rel_min = meand + rel_min * stddefd; // TODO
	rel_max = meand + rel_max * stddefd;

	for (auto it = input->begin(); it != input->end();)
	{
		if ((*(it).*get_data)() < rel_min || (*(it).*get_data)() > rel_max)
			input->erase(it);
		else
			++it;
	}
}

void Algo1Test::cleanup(Algorithm* base)
{
	/*int i = 0;
	for (void* stage : base->stack())
	{
		std::string name = "stage" + std::to_string(i++);

		cv::namedWindow(name, cv::WINDOW_NORMAL);
		cv::resizeWindow(name, 1920, 1080);
		cv::imshow(name, *reinterpret_cast<cv::Mat*>(stage));
	}

	while (cv::waitKey(0) != 'q')
		;
	cv::destroyAllWindows();*/
}
}
