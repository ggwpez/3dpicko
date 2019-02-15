#pragma once

#include <opencv2/core.hpp>

#include "include/algorithm.h"

namespace c3picko {
class Algo1Test : public Algorithm {
public:
  Algo1Test();
  virtual ~Algo1Test() override;
  virtual void run(void *const input, void *output) override;

  cv::Mat &new_stage();
  cv::Mat &new_stage(cv::Mat const &src);
  void render_rrect(cv::Mat &out, cv::RotatedRect rect);
  void drawText(cv::Mat &img, std::vector<cv::Vec3f> &colonies);
  bool roundness(cv::Mat &out, int area, int w, int h,
                 std::vector<cv::Point> const &contour, double roundness);

private:
  std::list<cv::Mat> stages_;
};
}
