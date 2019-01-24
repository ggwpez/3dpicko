#include "include/colonydetection.h"

using namespace c3picko;

std::vector<cv::Vec3f> ColonyDetection::algorithmFindContours(cv::Mat src, std::string settings_json){

	// create variables for images
	cv::Mat dest;
	src.copyTo(dest);
	cv::Mat& working = dest;

	// create container for variables
	std::vector<cv::Mat> contours;
	std::vector<cv::Vec3f> positions;

	// create Einstellungen
	DetectionSettings settings = Conversion::convertSettings(settings_json);


	// select settings for the preprocessing of the image
	settings.cvtColor.active = true;

	settings.adaptiveThreshold.active = true;

	// 11, 11
	settings.gaussianBlur.active = true;
	settings.gaussianBlur.kernelSize = cv::Size(11, 11);

	settings.dilate.active = true;
	settings.dilate.iterations = 1;

	settings.erode.active = true;
	settings.erode.iterations = 1;

	// 50, 125
	settings.canny.active = true;
	settings.canny.threshold1 = 10;
	settings.canny.threshold2 = 10;

	// start preprocessing
	Preprocessing::preprocessing(src, working, settings);


	// use find Contours algorithmn
	cv::findContours(working, contours, cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
	//cnts = cnts[0] if imutils.is_cv2() else cnts[1]

	for (size_t i = 0; i < contours.size(); i++)
	{

	}

	// get the size of the image
	int cols = src.cols;
	int rows = src.rows;

	// convert the positions of the colonies in procent
	return Conversion::createColonyCoordinates(positions, cols, rows);
}

std::vector<cv::Vec3f> ColonyDetection::colonyDetection(cv::Mat src, std::string settings_json){
	return algorithmHoughCircles(src, settings_json);
}

std::vector<cv::Vec3f> ColonyDetection::algorithmHoughCircles(cv::Mat src, std::string settings_json){

	// create variables for images
	cv::Mat dest;
	src.copyTo(dest);
	cv::Mat& working = dest;

	// create container for the colony positions
	std::vector<cv::Vec3f> positions;

	DetectionSettings settings = Conversion::convertSettings(settings_json);

	// select settings for the preprocessing of the image
	settings.cvtColor.active = true;

	settings.adaptiveThreshold.active = true;

	// 11, 11
	settings.gaussianBlur.active = true;
	settings.gaussianBlur.kernelSize = cv::Size(11, 11);

	settings.dilate.active = true;
	settings.dilate.iterations = 1;

	settings.erode.active = true;
	settings.erode.iterations = 1;

	// 50, 125
	settings.canny.active = true;
	settings.canny.threshold1 = 10;
	settings.canny.threshold2 = 10;

	// start preprocessing
	Preprocessing::preprocessing(src, working, settings);

	// use the hough circle algorithmn
	cv::HoughCircles(dest, positions, CV_HOUGH_GRADIENT, 1, 100, 20, 20, 0, 30);
	//cv::HoughCircles(src_grey, circles, CV_HOUGH_GRADIENT, 2, 10, 100, 20, 5, 10);

	// get the size of the image
	int cols = src.cols;
	int rows = src.rows;

	// convert the positions of the colonies in procent
	return Conversion::createColonyCoordinates(positions, cols, rows);
}
