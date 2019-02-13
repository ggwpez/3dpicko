#ifndef TEST_H
#define TEST_H

#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

namespace c3picko {

class Test {
public:
  static void imagePrint(cv::Mat img);
  static void testPrint(std::vector<cv::Vec3f> positions, cv::Mat &img);
};
} // namespace c3picko
#endif // TEST_H
