#ifndef IMAGEEDITING_H
#define IMAGEEDITING_H

#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <string>

namespace c3picko {

class ImageEditing {

  /**
   * @file The ImageEditing class is used to edit the image acording to the user
   * settings
   * @author Mayer, Patrick
   * @brief The Colonydetection class
   */
public:
  /**
   * @brief The imageEditing function crops and rotates the given image.
   * @param src: image.
   * @return settings_json: cropping and rotation information.
   */
  static void imageEditing(cv::Mat src, std::string settings_json);
};
} // namespace c3picko
#endif // IMAGEEDITING_H
