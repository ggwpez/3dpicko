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
  cv::Mat image_, preprocessed_;
};
