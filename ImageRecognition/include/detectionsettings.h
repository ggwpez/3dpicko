#ifndef DETECTIONSETTINGS_H
#define DETECTIONSETTINGS_H

#include <opencv2/core/core.hpp>

namespace c3picko {

/**
 * @file detectionSettings.h
 * @author Mayer, Patrick
 * @brief The detectionSettings struct is used to store settings.
 */

struct DetectionSettings {

  struct {
    bool active = false;
  } bitwiseNot;

  struct {
    bool active = false;
  } cvtColor;

  struct {
    bool active = false;
    cv::Size kernelSize;
    double sigX = 0;
    double sigY = 0;
  } gaussianBlur;

  struct {
    bool active = false;
    cv::Size var1;
    double var2;
  } adaptiveThreshold;

  struct {
    bool active = false;
    int threshold1;
    int threshold2;
  } canny;

  struct {
    bool active = false;
    int iterations;
  } dilate;

  struct {
    bool active = false;
    int iterations;
  } erode;
};
} // namespace c3picko

#endif // DETECTIONSETTINGS_H
