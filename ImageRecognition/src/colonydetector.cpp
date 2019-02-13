#include "../include/colonydetector.h"
#include "../include/test.h"
#include <list>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <set>
#include <unordered_set>

using namespace c3picko;

namespace c3picko {
ColonyDetector::ColonyDetector(cv::Mat source, QObject *_parent)
    : QObject(_parent), source_(source) {}

cv::Mat &ColonyDetector::new_stage() {
  cv::Mat new_mat;

  stages_.push_back(new_mat);
  return stages_.back();
}

cv::Mat &ColonyDetector::new_stage(cv::Mat const &src) {
  cv::Mat new_mat;
  src.copyTo(new_mat);

  stages_.push_back(new_mat);
  return stages_.back();
}

void ColonyDetector::render_rrect(cv::Mat &out, cv::RotatedRect rect) {
  cv::Point2f vertices2f[4];
  rect.points(vertices2f);

  cv::Point vertices[4];
  for (int i = 0; i < 4; ++i)
    vertices[i] = vertices2f[i];
  cv::fillConvexPoly(out, vertices, 4, cv::Vec3b(255, 255, 255));
}

void ColonyDetector::drawText(cv::Mat &img, std::vector<cv::Vec3f> &colonies) {
  cv::Point2i img_center{img.cols / 2, img.rows / 2};

  // Sort the colonies by ascending distance from the center
  std::sort(colonies.begin(), colonies.end(),
            [&img_center](cv::Vec3f const &a, cv::Vec3f const &b) {
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
    cv::putText(stages_.back(), text, t_pos, f_type, f_scale,
                cv::Scalar::all(255), f_thick);
  }
}

bool ColonyDetector::roundness(cv::Mat &out, int area, int w, int h,
                               std::vector<cv::Point> const &contour,
                               double roundness) {
  if ((std::min(w, h) / double(std::max(w, h))) < .7)
    return false;

  // TODO try watershed algorithm

  cv::RotatedRect rrect = cv::minAreaRect(contour);

  double ratio(std::min(rrect.size.width, rrect.size.height) /
               double(std::max(rrect.size.width, rrect.size.height)));
  if (ratio < .7 || std::isnan(ratio))
    return false;

  double circularity((4 * M_PI * area) /
                     std::pow(cv::arcLength(contour, true), 2));
  if (circularity < .6 || std::isnan(circularity)) // flouro .8
  {
    return false;
  }
  std::vector<cv::Point> hull, hull_contour;

  cv::convexHull(contour, hull, false, false);
  cv::approxPolyDP(hull, hull_contour, .001, true);

  double convexity(cv::contourArea(contour) / cv::contourArea(hull_contour));

  if (convexity < .8 || std::isnan(convexity)) // flouro .91
    return false;

  return true;
}

void ColonyDetector::run() {
  time_.start();

  algoFlour1("");
  pos_.resize(colonies_.size());
  for (std::size_t i = 0; i < colonies_.size(); ++i)
    pos_[i] = cv::Vec3d(colonies_[i].x(), colonies_[i].y(),
                        colonies_[i].circumference() / (2 * M_PI));

  emit OnFinished(time_.elapsed());
}

void ColonyDetector::algoFlour1(std::string settings_json) {
  Q_ASSERT(stages_.size() == 0 && colonies_.size() == 0 &&
           "Dont call more than once!");

  Colony::ID id = 0;
  // create variables for images
  cv::Mat stats, labeled, centers;
  cv::Scalar mean, stddev;

  cv::Mat &src = new_stage(source_);
  cv::Mat &grey = new_stage();
  // Greyscale
  cv::cvtColor(src, grey, CV_BGR2GRAY);

  // Calculate mean and standart deviation
  cv::Mat &grey_filtered = new_stage();
  // Filter by brightest pixels
  cv::adaptiveThreshold(grey, grey_filtered, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                        cv::THRESH_BINARY_INV, 51, 1); // non flour
  cv::meanStdDev(grey, mean, stddev);
  // cv::threshold(grey, grey_filtered, mean[0] +1.5*stddev[0], 255,
  // cv::THRESH_BINARY); // flour TODO tryp Otsu's method

  // Erode & Dilate
  int erosion_type = cv::MORPH_ELLIPSE;
  int erosion_size = 2;
  cv::Mat kernel = getStructuringElement(
      erosion_type, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
      cv::Point(erosion_size, erosion_size));

  cv::Mat &erroded = new_stage();
  grey_filtered.copyTo(erroded);
  cv::erode(grey_filtered, erroded, kernel);
  cv::dilate(erroded, erroded, kernel);

  // Label every connected component
  labeled = cv::Mat(erroded.rows, erroded.cols, CV_32S);
  cv::connectedComponentsWithStats(erroded, labeled, stats, centers);

  std::set<int> uniq_labels;
  cv::Mat &all_structs = new_stage(src);
  cv::Mat &labeled_stage = new_stage(src);
  for (int r = 0; r < labeled_stage.rows; ++r) {
    for (int c = 0; c < labeled_stage.cols; ++c) {
      int label = labeled.at<int>(r, c);
      if (!label)
        continue;
      int area = stats.at<int>(label, cv::CC_STAT_AREA);
      int w = stats.at<int>(label, cv::CC_STAT_WIDTH);
      int h = stats.at<int>(label, cv::CC_STAT_HEIGHT);
      int top = stats.at<int>(label, cv::CC_STAT_TOP);
      int left = stats.at<int>(label, cv::CC_STAT_LEFT);
      auto center = cv::Point2f(
          centers.at<double>(label, 0),
          centers.at<double>(
              label, 1)); // centers are doubles, despite the documentation!

      double round = .9;
      // Filter by area
      if (area > 1000 || area < 100)
        all_structs.at<cv::Vec3b>(r, c) = cv::Vec3b(255, 0, 0); // blau
      // Filter by roundness
      else {
        std::vector<std::vector<cv::Point>> contours;

        if ((std::min(w, h) / double(std::max(w, h))) < .7)
          continue;

        cv::Mat submat(erroded, cv::Rect(left, top, w, h));
        cv::findContours(submat, contours, cv::RETR_EXTERNAL,
                         cv::CHAIN_APPROX_SIMPLE);

        if (!roundness(all_structs, area, w, h, contours[0], round))
          all_structs.at<cv::Vec3b>(r, c) = cv::Vec3b(0, 0, 255); // rot
        else if (label) {
          labeled_stage.at<cv::Vec3b>(r, c) = cv::Vec3i(0, 255, 0);
          if (uniq_labels.find(label) == uniq_labels.end()) {
            Colony detected(center.x / double(source_.cols),
                            center.y / double(source_.rows), area,
                            cv::arcLength(contours[0], true), 0, 0, ++id,
                            Colony::Type::INCLUDED);
            colonies_.push_back(detected);
            // pos.push_back(cv::Vec3f(center.x, center.y, (float(w)+h)/2));
            uniq_labels.insert(label);
          }
        }
      }
    }
  }

  /*std::cout << "good bois: " << uniq_labels.size() << std::endl;
  drawText(stages_.back(), pos);

  int i = 0;
  for (cv::Mat& stage : stages_)
  {
          std::string name = "stage" +std::to_string(i++);

          cv::namedWindow(name, cv::WINDOW_NORMAL);
          cv::resizeWindow(name, 1920,1080);
          cv::imshow(name, stage);
  }

  while (cv::waitKey(0) != 'q');
  cv::destroyAllWindows();*/
}

std::vector<Colony> ColonyDetector::colonies() const { return colonies_; }

std::vector<cv::Vec3f> ColonyDetector::pos() const { return pos_; }
} // namespace c3picko
