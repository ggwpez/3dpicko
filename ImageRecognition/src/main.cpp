#include "include/main.h"

using namespace c3picko;

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

int main(int argc, char** argv)
{
    // Überprüfe ob eine Datei übergeben wurde
    if (argc != 2)
    {
        std::cerr << "First argument must be a path to an image" <<std::endl;
        return -1;
    }

    // Erstelle zwei Biler, src und dest
        // src: Quellbild
        // preprocessed: Umgewandeltes Bild
        // dest: Zielbild
        // opt: Optionen, Einstellungen für den Algorithmus
    cv::Mat src, preprocessed, dest;

    // Coordinaten
    std::vector<cv::Vec3f> positions;

    // lade Bild in src
    //src = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
    src = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

    // überprüfe ob es sich um ein Bild handelt
    if (! src.data)
    {
        std::cerr << "Image not found" << std::endl;
        return -1;
    }

    std::string settings_json;

    // starte Erkennung übergebe src und out
    ColonyDetection::colonyDetection(src, settings_json);
    //positions = algorithmFindContours(preprocessed, dest);

    // Exportiere Coordinaten
    //writeCoordinatesOut(positions, cols, rows);

    // starte GUI
    {
        cv::namedWindow("colonies", cv::WINDOW_NORMAL);
        cv::resizeWindow("colonies", 1024,768);
        cv::imshow("colonies", dest);
    }
    cv::waitKey(0);
    return 0;
}

// TESTESTEST
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



/*
#include <QCoreApplication>
#include <QSettings>
#include "httplistener.h"
#include "requestmapper.h"


// includes für colony detection:
#include "colonydetection.h"
#include "dishdetection.h"

using namespace stefanfrings;

int main(int argc, char *argv[])
{
    //algorithmHoughCircles(argv[1]);


    QCoreApplication app(argc, argv);

    // Load the configuration file
    QString configFileName=("../ImageRecognition/etc/webapp.ini");
    QSettings* listenerSettings=new QSettings(configFileName, QSettings::IniFormat, &app);
    listenerSettings->beginGroup("listener");

    // Static file controller
    QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    fileSettings->beginGroup("files");
    RequestMapper::staticFileController=new StaticFileController(fileSettings,&app);

    // Start the HTTP server
    //new HttpListener(listenerSettings, new RequestMapper(&app), &app);

    return app.exec();
}
*/

/*static void algorithmHoughCircles(cv::Mat src, std::string settings_json){

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
    Conversion::createColonyCoordinates(positions, cols, rows);
}
*/
