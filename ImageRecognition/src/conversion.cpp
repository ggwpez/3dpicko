#include "../include/conversion.h"

using namespace c3picko;

std::vector<cv::Vec3f> Conversion::createColonyCoordinates(std::vector<cv::Vec3f> positions, int cols, int rows)
{
	std::vector<cv::Vec3f> positions_percent;

	for (size_t i = 0; i < positions.size(); i++){
		positions_percent.push_back({ positions[i][0] / (cols / 100),
									  positions[i][1] / (rows / 100),
									  positions[i][2]});
	}
	// Dummies for testing
	positions_percent.push_back({0,0,20});
	positions_percent.push_back({100, 100, 20});
	positions_percent.push_back({0, 100, 20});
	positions_percent.push_back({100, 0, 20});

	return positions_percent;
}

std::vector<cv::Vec3f> Conversion::createDishCoordinates(std::vector<cv::Vec3f> positions, int cols, int rows){
return positions;
}

DetectionSettings Conversion::convertSettings(std::string settings_json){

	DetectionSettings settings;

}
