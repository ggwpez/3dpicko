#include "include/algorithms/normal1.h"
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "include/algo_setting.h"
#include "include/algorithm_job.h"
#include "include/algorithm_result.h"
#include "include/algorithms/helper.h"
#include "include/colony.hpp"

namespace c3picko {

Normal1::Normal1()
    : Algorithm(
          "1", "Normal1", "Detects colonies with standard illumination",
          {(AlgoStep)&Normal1::cvt, (AlgoStep)&Normal1::threshold,
           (AlgoStep)&Normal1::erodeAndDilate, (AlgoStep)&Normal1::label /*,
		   (AlgoStep)&Normal1::relativeFiltering*/},
          {/*AlgoSetting::make_checkbox("show_excluded_by_algo",
                                                                                                                                                                                                                                                                                                                                                                                                                                                                          "Show ignored
                                          by algorithm", "", true, {},
              Qt::red),*/
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
          true, 3000) {}

Normal1::~Normal1() {}

void Normal1::render_rrect(cv::Mat& out, cv::RotatedRect rect) {
  cv::Point2f vertices2f[4];
  rect.points(vertices2f);

  cv::Point vertices[4];
  for (int i = 0; i < 4; ++i) vertices[i] = vertices2f[i];
  cv::fillConvexPoly(out, vertices, 4, cv::Vec3b(255, 255, 255));
}

void Normal1::drawText(cv::Mat& img, cv::Mat& output,
                       std::vector<cv::Vec3f>& colonies) {
  cv::Point2i img_center{img.cols / 2, img.rows / 2};

  // Sort the colonies by ascending distance from the center
  std::sort(colonies.begin(), colonies.end(),
            [&img_center](cv::Vec3f const& a, cv::Vec3f const& b) {
              return (std::pow(img_center.x - a[0], 2) +
                      std::pow(img_center.y - a[1], 2)) <
                     (std::pow(img_center.x - b[0], 2) +
                      std::pow(img_center.y - b[1], 2));
            });

  int f_type = cv::FONT_HERSHEY_SIMPLEX;
  int f_scale = 2;
  int f_thick = 2;

  for (int i = 0; i < colonies.size(); ++i) {
    std::string text = std::to_string(i);
    cv::Point pos{int(colonies[i][0]), int(colonies[i][1])};

    cv::Size text_size =
        cv::getTextSize(text, f_type, f_scale, f_thick, nullptr);
    cv::Point t_pos = {pos.x - text_size.width / 2,
                       pos.y + text_size.height / 2};
    cv::putText(output, text, t_pos, f_type, f_scale, cv::Scalar::all(255),
                f_thick);
  }
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
  {
    return "circularity";
  }
  std::vector<cv::Point> hull, hull_contour;

  cv::convexHull(contour, hull, false, false);
  cv::approxPolyDP(hull, hull_contour, .001, true);

  double convex(cv::contourArea(contour) / cv::contourArea(hull_contour));

  return convexity.contains(convex) ? "" : "convexity";  // flouro .91
}

void Normal1::cvt(AlgorithmJob* base, DetectionResult* result) {
  result->newMat();

  cv::Mat* input = reinterpret_cast<cv::Mat*>(base->input());
  cv::cvtColor(*input, result->newMat(), CV_BGR2GRAY);
}

void Normal1::threshold(AlgorithmJob* base, DetectionResult* result) {
  // Calculate mean and standart deviation
  cv::Mat& input = result->oldMat();
  cv::Mat& output = result->newMat();

  cv::Scalar mean, stddev;
  cv::adaptiveThreshold(input, output, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                        cv::THRESH_BINARY_INV, 51, 1);  // non flour
}

void Normal1::erodeAndDilate(AlgorithmJob* base, DetectionResult* result) {
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

void Normal1::plateDetection(AlgorithmJob* base, DetectionResult* result) {
  cv::Mat& erroded = result->oldMat();
  // cv::Mat& output  =
  // result->newMat(*reinterpret_cast<cv::Mat*>(base->input()));
  cv::Mat output(*reinterpret_cast<cv::Mat*>(base->input()));

  if (!base->settingById("plate_detection").value<bool>()) return;

  cv::Mat canny_out;
  std::vector<std::vector<cv::Point>> conts;

  cv::Canny(erroded, canny_out, 3, 11);
  cv::findContours(canny_out, conts, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

  std::vector<math::LineSeg> lines;
  // Fit lines throug the contours and calculate the angle and length to image
  // coordinate system
  for (std::size_t i = 0; i < conts.size(); ++i) {
    cv::RotatedRect rect = cv::minAreaRect(conts[i]);

    double r(std::min(rect.size.width, rect.size.height) /
             double(std::max(rect.size.width, rect.size.height)));
    if (r > .05 || std::isnan(r) || cv::arcLength(conts[i], false) < 10)
      continue;

    cv::Vec4d fit_line;
    // cv::drawContours(result, conts, i, color, 2, 8);
    cv::fitLine(conts[i], fit_line, CV_DIST_L2, 0, .01, .01);

    double length =
        std::sqrt(std::pow(rect.size.width, 2) + std::pow(rect.size.height, 2));
    cv::Point2d v =
        cv::Point2d(fit_line[0] * length / 2, fit_line[1] * length / 2);
    // cv::fitLine matches the lines sometimes form top-to-bottom and sometimes
    // from bottom-to-top
    // so we need to turn the wrong ones around.
    if (fit_line[1] < 0) {
      v = -v;
      fit_line[1] = -fit_line[1];
    }

    cv::Point2d start = cv::Point2d(fit_line[2], fit_line[3]) - v;
    cv::Point2d send = cv::Point2d(fit_line[2], fit_line[3]) + v;

    math::LineSeg line{
        start.x, start.y, send.x, send.y, {fit_line[0], fit_line[1]}, length};
    lines.push_back(line);
  }

  std::vector<cv::Scalar> bin_clrs;
  cv::Mat labels;
  std::vector<cv::Point2f> centers;
  // kmeans only works with floats
  std::vector<cv::Point2f> points(lines.size(), cv::Point2f());
  cv::TermCriteria crit(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1);

  for (std::size_t i = 0; i < lines.size(); ++i) {
    cv::Point2d p(-lines[i].angle.imag(), lines[i].angle.real());
    // std::cout << (std::arg(lines[i].angle) / M_PI) * 180.f << "°\n";
    points[i] = p;
    cv::line(
        output, cv::Point(output.cols / 2, output.rows / 2),
        cv::Point2f(output.cols / 2 + p.x * 1000, output.rows / 2 + p.y * 1000),
        cv::Scalar::all(255), 1);
  }

  int const ks = 3;
  // We need atleast as many points as possible centers
  if (points.size() < ks) return;
  cv::kmeans(points, ks, labels, crit, 3, cv::KMEANS_PP_CENTERS, centers);

  for (int i = 0; i < ks; ++i)
    bin_clrs.push_back(cv::Scalar(rand() % 255, rand() % 255, rand() % 255));

  cv::circle(output, cv::Point2f(output.cols / 2, output.rows / 2), 10,
             cv::Scalar::all(255), 4);
  for (auto const& center : centers) {
    cv::line(output, cv::Point2f(output.cols / 2, output.rows / 2),
             cv::Point2f(output.cols / 2, output.rows / 2) +
                 cv::Point2f(center.x * 60, center.y * 60),
             cv::Scalar(0, 0, 255), 3);
    // std::cout << "c " << center.x << " / " << center.y << "\n";
  }

  for (std::size_t i = 0; i < lines.size(); ++i) {
    cv::line(output, cv::Point(lines[i].x, lines[i].y),
             cv::Point(lines[i].endx, lines[i].endy),
             bin_clrs[labels.at<int>(i)], 2);
  }
}

void Normal1::label(AlgorithmJob* base, DetectionResult* result) {
  cv::Mat& input = result->oldMat();
  auto& colonies = result->colonies_;

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
                              center.y / double(input.rows), 0, 0, 10, 0, ++id,
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
                            std::sqrt(area / M_PI), 0, ++id, "area");
      continue;
    }
    // Filter by roundness
    else {
      std::vector<std::vector<cv::Point>> contours;

      // We check the AABB_RATIO twice for speed NOTE look into this in
      // benchmark phase
      if (_aabb_ratio.excludes(std::min(w, h) / double(std::max(w, h)))) {
        colonies.push_back(
            Colony(center.x / double(input.cols), center.y / double(input.rows),
                   area, 0, std::sqrt(area / M_PI), 0, ++id, "aabb_ratio"));
      }

      cv::Mat submat(input, cv::Rect(left, top, w, h));
      cv::findContours(submat, contours, cv::RETR_EXTERNAL,
                       cv::CHAIN_APPROX_SIMPLE);

      AlgoSetting::ID excluded_by =
          roundness(area, w, h, contours[0], _aabb_ratio, _bb_ratio,
                    _circularity, _convexity);

      if (!excluded_by.isEmpty())
        colonies.push_back(
            Colony(center.x / double(input.cols), center.y / double(input.rows),
                   area, 0, std::sqrt(area / M_PI), 0, ++id, excluded_by));
      else {
        double circ = cv::arcLength(contours[0], true);

        // NOTE Brightness only works on unmasked original image contours,
        // otherwise they need conversion
        double br = math::brightness(
            contours[0], *reinterpret_cast<cv::Mat*>(base->input()));
        Colony detected(center.x / double(input.cols),
                        center.y / double(input.rows), area, circ,
                        circ / (2 * M_PI), br, ++id, "");
        colonies.push_back(detected);
      }
    }
  }
}

void Normal1::relativeFiltering(AlgorithmJob* base, DetectionResult* result) {
  std::vector<Colony>* input = &result->colonies_;
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

}  // namespace c3picko
