#include "../include/main.h"

using namespace c3picko;


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
        // dest: Zielbild
    cv::Mat src;

    // Coordinaten
    std::vector<cv::Vec3f> positions;

    src = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

    // überprüfe ob es sich um ein Bild handelt
    if (! src.data)
    {
        std::cerr << "Image not found" << std::endl;
        return -1;
    }

    std::string settings_json;
    
    ColonyDetection::algorithmHoughCircles(src, settings_json);

    return 0;
}




