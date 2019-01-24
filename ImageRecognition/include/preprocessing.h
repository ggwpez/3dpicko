#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

#include <fstream>

#include "detectionsettings.h"

namespace c3picko {

    /**
     * @file detectionsettings.h
     * @author Mayer, Patrick
     * @brief The Preprocessing class is used to preprocess images
     */

    class Preprocessing {
    public:

        /**
         * @brief The preprocessing function is used to preporcess the image so that the image recognition can work
         * @param src: original image.
         * @param dest: manupilated image.
         * @param settings: settings to be applied.
         */
        static void preprocessing(cv::Mat src, cv::Mat& dest, struct DetectionSettings settings);
    };
}

#endif // PREPROCESSING_H
