#ifndef COLONYDETECTION_H
#define COLONYDETECTION_H

#include <opencv2/core/core.hpp>

#include <string>

#include "conversion.h"
#include "detectionsettings.h"
#include "preprocessing.h"

namespace c3picko {

	/**
	 * @file colonydetection.h
	 * @author Mayer, Patrick
	 * @brief The Colonydetection class is responsible for detecting colonies.
	 */
	class ColonyDetection {
	public:

		/**
		 * @brief The algorithmFindContours function:
		 * @param src: Image of colonies
		 * @param settings_json: Custom settings which should be applied to the image for better results.
		 * @return void
		 */
		static std::vector<cv::Vec3f> algorithmFindContours(cv::Mat src, std::string settings_json);

		/**
		 * @brief The algorithmHoughCircles function
		 * @param src: Image of colonies
		 * @param settings_json: Custom settings which should be applied to the image for better results.
		 * @return void
		 */
		static std::vector<cv::Vec3f> algorithmHoughCircles(cv::Mat src, std::string settings_json);

		/**
		 * @brief The colonydetection function
		 * @param src: Image of colonies
		 * @param settings_json: Custom settings which should be applied to the image for better results.
		 * @return void
		 */
		static std::vector<cv::Vec3f> colonyDetection(cv::Mat src, std::string settings_json);
	};
}

#endif // COLONYDETECTION_H
