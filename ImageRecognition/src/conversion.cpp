#include "../include/conversion.h"

using namespace c3picko;

std::vector<cv::Vec3f> Conversion::createColonyCoordinates(std::vector<cv::Vec3f> positions, int cols, int rows){
	/*std::fstream file;
	file.open ("coorinates.json");
	file << "{\"colonies\":[\n";
	for(size_t i = 0; i < positions.size(); i++){
		file << "{\"xCoordinate\":\""+ std::to_string( static_cast<double>(positions[i][0])/(cols / 100)) +
		"\",\"yCoordinate\":\"" + std::to_string( static_cast<double>(positions[i][1])/(rows / 100)) +
		"\", \"diameter\":\"" + std::to_string( static_cast<double>(positions[i][2])/(rows / 100)) + "\" },\n";
	}
	file << "]}";
	file.close();*/

	std::vector<cv::Vec3f> positions_percent;
	positions_percent.resize(positions.size());
	for (size_t i = 0; i < positions.size(); i++){
		positions_percent[i][0] = (positions[i][0]) / (cols / 100);
		positions_percent[i][1] = (positions[i][1]) / (rows / 100);
		positions_percent[i][2] = (positions[i][2]) / (rows / 100);
	}
	return positions_percent;
}

std::vector<cv::Vec3f> Conversion::createDishCoordinates(std::vector<cv::Vec3f> positions, int cols, int rows){
return positions;
}

DetectionSettings Conversion::convertSettings(std::string settings_json){

	DetectionSettings settings;

}
