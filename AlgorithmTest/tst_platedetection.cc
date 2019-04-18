#include "tst_platedetection.h"
#include <QtTest>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "include/algorithms/helper.h"
#include "include/colony.h"

using namespace c3picko;

PlateDetection::PlateDetection() {}

PlateDetection::~PlateDetection() {}

void PlateDetection::initTestCase() {
  QDirIterator it(dir_path_);
  while (it.hasNext()) {
    QString file_name(it.next());
    if (!file_name.toLower().contains(".jpg", Qt::CaseInsensitive)) continue;

    cv::Mat original = cv::imread(file_name.toStdString()), preprocessed;

    cv::cvtColor(original, preprocessed, CV_BGR2GRAY);
    cv::adaptiveThreshold(preprocessed, preprocessed, 255,
                          cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 51,
                          1);  // non flour

    images_.emplace_back(original, preprocessed);
  }
}

void PlateDetection::cleanupTestCase() {}

void PlateDetection::test_case1() {
  int i = 0;
  for (auto const &it : images_) {
    try {
      /*cv::Mat out = math::detectPlate2(it.first, it.second);
      cv::imwrite("out_" + std::to_string(i++) + ".png", out);
      cv::resize(out, out, cv::Size(out.cols / 2, out.rows / 2));
      cv::imshow("", out);

      while (cv::waitKey(0) != 'q')
              ;
      cv::destroyAllWindows();*/
    } catch (std::exception const &e) {
      qCritical() << e.what();
      continue;
    }
  }

  /*cv::imshow("detected", image_);

  while (cv::waitKey(0) != 'q')
          ;
  cv::destroyAllWindows();*/
}
