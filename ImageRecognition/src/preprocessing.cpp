#include "../include/preprocessing.h"

using namespace c3picko;

void Preprocessing::preprocessing(cv::Mat src, cv::Mat &dest,
                                  struct DetectionSettings settings) {

  // create the image to be worked on
  cv::Mat working;
  cv::Mat working_bitwiseNot;
  cv::Mat working_adaptiveThreshold;
  cv::Mat working_gaussianBlur;
  cv::Mat working_dilate;
  cv::Mat working_erode;
  cv::Mat working_canny;

  src.copyTo(working);

  // convert a colored image in to a grey one
  if (settings.bitwiseNot.active) {
    cv::cvtColor(working, working_bitwiseNot, CV_BGR2GRAY);
  } else {
    working.copyTo(working_bitwiseNot);
  }

  if (settings.adaptiveThreshold.active) {
    // Zahlen nur Platzhalter
    cv::adaptiveThreshold(working_bitwiseNot, working_adaptiveThreshold, 2, 2,
                          2, 2, 2);
  } else {
    working_bitwiseNot.copyTo(working_adaptiveThreshold);
  }

  // gaussian blur
  // GaussianBlur(input, output, kernel size (pixels around it),Gaussian kernel
  // standard deviation in X direction,Gaussian kernel standard deviation in Y
  // direction)
  if (settings.gaussianBlur.active) {
    cv::GaussianBlur(working_adaptiveThreshold, working_gaussianBlur,
                     settings.gaussianBlur.kernelSize,
                     settings.gaussianBlur.sigX, settings.gaussianBlur.sigY);
  } else {
    working_adaptiveThreshold.copyTo(working_gaussianBlur);
  }

  // dilate
  // dilate(input, output, structuring element used for dilation (3x3 rectagle),
  // position of anchor within the element (-1 , -1 means center), number of
  // times it is applied
  if (settings.dilate.active) {
    cv::dilate(working_gaussianBlur, working_dilate, cv::Mat(),
               cv::Point(-1, -1), settings.dilate.iterations);
  } else {
    working_gaussianBlur.copyTo(working_dilate);
  }

  // erode
  // erode(input, output, structuring element used for dilation (3x3 rectagle),
  // position of anchor within the element (-1 , -1 means center), number of
  // times it is applied
  if (settings.erode.active) {
    cv::erode(working_dilate, working_erode, cv::Mat(), cv::Point(-1, -1),
              5); // settings.erode.iterations
  } else {
    working_dilate.copyTo(working_erode);
  }

  // canny algorithmn
  // cammy((input, output, first threshold for the hysteresis procedure, second
  // threshold for the hysteresis procedure)
  if (settings.canny.active) {
    cv::Canny(working_erode, working_canny, settings.canny.threshold1,
              settings.canny.threshold2);
  } else {
    working_erode.copyTo(working_canny);
  }

  working_canny.copyTo(dest);
}
