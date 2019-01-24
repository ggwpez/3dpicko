#ifndef MAIN_H
#define MAIN_H

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "colonydetection.h"
#include "conversion.h"
#include "detectionsettings.h"
#include "preprocessing.h"

namespace c3picko {

    /**
     * @file main.h
     * @author Mayer, Patrick
     * @brief The main class is for testing purposes
     */

    class Main {
    public:

        /**
         * @file
         * @author Mayer, Patrick
         * @brief The Colonydetection class
         * @param
         * @return
         */
        void testPrint(std::vector<cv::Vec3f> positions, cv::Mat img);

        /**
         * @file
         * @author Mayer, Patrick
         * @brief The Colonydetection class
         * @param
         * @return
         */
        int main(int argc, char** argv);
    };
}

#endif // MAIN_H
