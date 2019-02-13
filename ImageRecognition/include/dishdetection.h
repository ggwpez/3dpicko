#ifndef DISHDETECTION_H
#define DISHDETECTION_H

#include <opencv2/core/core.hpp>

#include "detectionsettings.h"
#include "preprocessing.h"

namespace c3picko {

/**
 * @file dishdetection.h
 * @author Mayer, Patrick
 * @brief The Colonydetection class
 */

class Dishdetection {

public:
  /**
   * @brief The dishdetection function is responsible for detecting the
   * coordinates of the petri dish's edge.
   * @param petridish
   * @param outerBox
   */
  static void dishdetection(cv::Mat petridish, cv::Mat outerBox);
};
} // namespace c3picko

#endif // DISHDETECTION_H
