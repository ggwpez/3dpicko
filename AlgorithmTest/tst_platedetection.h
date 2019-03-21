#pragma once

#include <QObject>
#include <opencv2/core/mat.hpp>

class PlateDetection : public QObject {
  Q_OBJECT

public:
  PlateDetection();
  ~PlateDetection();

private slots:
  void initTestCase();
  void cleanupTestCase();
  void test_case1();

private:
  QString const dir_path_ = "/home/vados/src/bc_pics/mit rahmen/undist/";
  // original, preprocessed
  std::vector<std::pair<cv::Mat, cv::Mat>> images_;
};
