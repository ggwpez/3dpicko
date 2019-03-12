#include "tst_platedetection.h"
#include "include/algorithms/helper.h"
#include "include/colony.hpp"
#include <QtTest>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace c3picko;

PlateDetection::PlateDetection() {}

PlateDetection::~PlateDetection() {}

void PlateDetection::initTestCase()
{
	cv::Mat grey;
	image_ = cv::imread("/home/vados/Pictures/mit rahmen/PICT_20190308_172849.JPG_undistored");

	cv::cvtColor(image_, grey, CV_BGR2GRAY);

	int		erosion_type = cv::MORPH_ELLIPSE;
	int		erosion_size = 2;
	cv::Mat kernel
		= getStructuringElement(erosion_type, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), cv::Point(erosion_size, erosion_size));

	cv::adaptiveThreshold(grey, preprocessed_, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 51, 1); // non flour
	// cv::blur(grey, preprocessed_, cv::Size(5, 5));
	// cv::erode(preprocessed_, preprocessed_, kernel);
	// cv::dilate(preprocessed_, preprocessed_, kernel);
}

void PlateDetection::cleanupTestCase() { cv::imwrite("undistorted_125.jpg", image_); }

void PlateDetection::test_case1()
{
	math::detectPlate2(image_, preprocessed_);

	cv::imshow("detected", image_);

	while (cv::waitKey(0) != 'q')
		;
	cv::destroyAllWindows();
}
