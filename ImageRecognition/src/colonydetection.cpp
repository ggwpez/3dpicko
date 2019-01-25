#include "../include/colonydetection.h"

using namespace c3picko;

std::vector<cv::Vec3f> ColonyDetection::algorithmFindContours(cv::Mat src, std::string settings_json){

	// create variables for images
	cv::Mat dest;
    src.cv::Mat::copyTo(dest);
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

        /*
        Make Contours into vector
        // Get the moments
        std::vector<cv::Moments> mu(contours.size());

        for( int i = 0; i < contours.size(); i++ ) {
            mu[i] = moments( contours[i], false );
        }

        //  Get the mass centers:
        std::vector<cv::Point2f> mc( contours.size() );
        for( int i = 0; i < contours.size(); i++ ) {
            mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
        }
        */

        // get the size of the image
        int cols = src.cols;
        int rows = src.rows;

        // convert the positions of the colonies in procent
        return Conversion::createColonyCoordinates(positions, cols, rows);
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

        settings.adaptiveThreshold.active = false;

        // 11, 11
        settings.gaussianBlur.active = true;
        settings.gaussianBlur.kernelSize = cv::Size(11, 11);

        settings.dilate.active = true;
        settings.dilate.iterations = 1;

        settings.erode.active = true;
        settings.erode.iterations = 1;

        // 50, 125
        settings.canny.active = true;
        settings.canny.threshold1 = 11;
        settings.canny.threshold2 = 11;

        // start preprocessing
        Preprocessing::preprocessing(src, working, settings);

        //Test::imagePrint(working);

        // use the hough circle algorithmn
        cv::HoughCircles(dest, positions, CV_HOUGH_GRADIENT, 1, 100, 20, 20, 0, 30);
        //cv::HoughCircles(src_grey, circles, CV_HOUGH_GRADIENT, 2, 10, 100, 20, 5, 10);

        // get the size of the image
        int cols = src.cols;
        int rows = src.rows;

        cv::namedWindow("src", cv::WINDOW_NORMAL);
        cv::resizeWindow("src", 1024,768);
        cv::imshow("src", src);

        cv::waitKey(0);
        cv::destroyWindow("src");


        cv::namedWindow("working", cv::WINDOW_NORMAL);
        cv::resizeWindow("working", 1024,768);
        cv::imshow("working", working);

        cv::waitKey(0);
        cv::destroyWindow("working");


        cv::namedWindow("dest", cv::WINDOW_NORMAL);
        cv::resizeWindow("dest", 1024,768);
        cv::imshow("dest", dest);

        cv::waitKey(0);
        cv::destroyWindow("dest");

        // convert the positions of the colonies in procent
        return Conversion::createColonyCoordinates(positions, cols, rows);

        }

        std::vector<cv::Vec3f> ColonyDetection::colonyDetection(cv::Mat src, std::string settings_json){
            return algorithmHoughCircles(src, settings_json);
        }
