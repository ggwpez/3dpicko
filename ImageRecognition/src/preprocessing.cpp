#include "include/preprocessing.h"

using namespace c3picko;

void Preprocessing::preprocessing(cv::Mat src, cv::Mat& dest, struct DetectionSettings settings){

	// create the image to be worked on
	cv::Mat working;
	src.copyTo(working);

	// convert a colored image in to a grey one
	if (settings.bitwiseNot.active){
		cv::cvtColor(working, working, CV_BGR2GRAY);
	}

	// TBD
	if (settings.adaptiveThreshold.active){
		// Zahlen nur Platzhalter
		//cv::adaptiveThreshold(working, working, 2, 2,2,2,2);
	}

	// gaussian blur
	// GaussianBlur(input, output, kernel size (pixels around it),Gaussian kernel standard deviation in X direction,Gaussian kernel standard deviation in Y direction)
	if (settings.gaussianBlur.active){
		cv::GaussianBlur(working, working, settings.gaussianBlur.kernelSize, settings.gaussianBlur.sigX, settings.gaussianBlur.sigY);
	}

	// dilate
	// dilate(input, output, structuring element used for dilation (3x3 rectagle), position of anchor within the element (-1 , -1 means center), number of times it is applied
	if (settings.dilate.active){
		cv::dilate(working, working, cv::Mat(), cv::Point(-1,-1), settings.dilate.iterations);
	}

	// erode
	// erode(input, output, structuring element used for dilation (3x3 rectagle), position of anchor within the element (-1 , -1 means center), number of times it is applied
	if (settings.erode.active){
		cv::erode(working, working, cv::Mat(), cv::Point(-1,-1), settings.erode.iterations);
	}

	// canny algorithmn
	//cammy((input, output, first threshold for the hysteresis procedure, second threshold for the hysteresis procedure)
	if (settings.canny.active){
		cv::Canny(working, working, settings.canny.threshold1, settings.canny.threshold2);
	}

	// copy to export
	working.copyTo(dest);

	// TEST TEST TEST TEST TEST
	/*{
		cv::namedWindow("preprocessed", cv::WINDOW_NORMAL);
		cv::resizeWindow("preprocessed", 1024,768);
		cv::imshow("preprocessed", dest);
	}
	cv::waitKey(0);
	cv::destroyWindow("preprocessed");*/
}
