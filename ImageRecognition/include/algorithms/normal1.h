#pragma once

#include <opencv2/core.hpp>
#include "include/colony.hpp"

#include "include/algorithm.h"

namespace c3picko {
class Normal1 : public Algorithm {
  Q_OBJECT
 public:
  Normal1();
  virtual ~Normal1() override;

  static void cvt(AlgorithmJob* base, AlgorithmResult* result);
  static void threshold(AlgorithmJob* base, AlgorithmResult* result);
  static void erodeAndDilate(AlgorithmJob* base, AlgorithmResult* result);
  static void plateDetection(AlgorithmJob* base, AlgorithmResult* result);
  static void label(AlgorithmJob* base, AlgorithmResult* result);
  static void relativeFiltering(AlgorithmJob* base, AlgorithmResult* result);

  virtual inline Algorithm* cloneEmpty() const override {
    return new Normal1();
  }

  void render_rrect(cv::Mat& out, cv::RotatedRect rect);
  void drawText(cv::Mat& img, cv::Mat& output,
                std::vector<cv::Vec3f>& colonies);

 private:
};
}  // namespace c3picko
