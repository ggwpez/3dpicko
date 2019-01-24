#ifndef CONVERSION_H
#define CONVERSION_H

#include <opencv2/core/core.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include "detectionsettings.h"

namespace c3picko {

    /**
     * @file conversion.h
     * @author Mayer, Patrick
     * @brief The Conversion class is responsible for converting settings and coordinates.
     */

    class Conversion     {
    public:

        /**
         * @brief The createColonyCoordinates function: creates the positions of the colonies.
         * @param positions: Positions of the center of colonies.
         * @param cols: Number of columns of the image.
         * @param rows: Number of rows of the image.
         * @return positions: positions of the colonies in percentage.
         */
        static std::vector<cv::Vec3f> createColonyCoordinates(std::vector<cv::Vec3f> positions, int  cols, int rows);

        /**
         * @brief The createDishCoordinates function: creates the positions of the petri dish.
         * @param positions: Positions of edges of the petri dish.
         * @param cols: Number of columns of the image.
         * @param rows: Number of rows of the image.
         * @return positions: positions of the colonies in percentage.
         */
        static std::vector<cv::Vec3f> createDishCoordinates(std::vector<cv::Vec3f> positions, int cols, int rows);

        /**
         * @brief The convertSettings function: converts settings from json to detectionSettings.
         * @param settings_json: Custom settings to be applied to the image.
         * @return settings: Custom settings converted to detectionSettings.
         */
        static DetectionSettings convertSettings(std::string settings_json);
    };
}
#endif // CONVERSION_H
