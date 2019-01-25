#include "../include/test.h"

using namespace c3picko;

static void imagePrint(cv::Mat& output){

    cv::namedWindow("preprocessed", cv::WINDOW_NORMAL);
    cv::resizeWindow("preprocessed", 1024,768);
    cv::imshow("preprocessed", output);

    cv::waitKey(0);
    cv::destroyWindow("preprocessed");
}

void testPrint(std::vector<cv::Vec3f> positions, cv::Mat img){

    // Zeiche Positionen
    for(size_t i = 0; i < positions.size(); i++)
    {
        cv::Point center(cvRound(positions[i][0]), cvRound(positions[i][1]));
        int radius = cvRound(positions[i][2]);

        // circle center
        cv::circle(img, center, 3, cv::Scalar(255,0,0), 1, 8, 0);

        // circle outline
        cv::circle(img, center, radius, cv::Scalar(255,0,0), 3, 8, 0);
    }

    // Zeige Anzahl an Positionen an
    std::cout << "Found cirles: " << positions.size() << std::endl;
}

/*
for (size_t i = 0; i < positions.size(); i++)
{
    // maximalgröße der Kolonie
    if (cv::contourArea(i) < 5)
        continue;

    //was?
    cv::Mat hull;

    //was?
    cv::convexHull(i, hull);

    cv::drawContours(dest, {hull}, 0, cv::Scalar(0,0,255), 10);
}

std::cout << "Kolnien: " << positions.size() << std::endl;
*/
