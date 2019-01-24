#include "include/dishdetection.h"

using namespace c3picko;

static void dishdetection(cv::Mat petridish, cv::Mat outerBox){

    // Konvertierung schwarz weiß
    cv::cvtColor(petridish, petridish, CV_BGR2GRAY);

    // Weichzeichner (Eingabe, Ausgabe, ?, ?, ?)
    cv::GaussianBlur(petridish, petridish, cv::Size(11, 11), 0);

    //cv::adaptiveThreshold(*petridish, *outerBox, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);

    //invertieren?
    bitwise_not(outerBox, outerBox);

    cv::Mat petri;

    //createsettings
    DetectionSettings settings;
    settings.cvtColor.active = true;

    settings.gaussianBlur.active = true;
    settings.gaussianBlur.kernelSize = cv::Size(11, 11);

    settings.adaptiveThreshold.active = true;

    settings.bitwiseNot.active = true;

    // preprocessing
    Preprocessing::preprocessing(petridish, petri, settings);

    //Löcher auf dem Rande der Schale Füllen um sodann die gesammte Kante zu erkennen
    cv::Mat kernel = (cv::Mat_<uchar>(3,3) << 0,1,0,1,1,1,0,1,0);
    cv::dilate(outerBox, outerBox, kernel);
}
