#include "include/printerprofile.h"

namespace c3picko {

	PrinterProfile::PrinterProfile(const QJsonObject& obj)
		: JsonConstructable(QJsonObject()),
		  cut_filament_position_(Point(obj["cut_filament_position"].toObject())),
		  filament_extrusion_length_(obj["filament_extrusion_length"].toDouble()),
		  z_coordinate_extruded_filament_above_plates_(obj["z_coordinate_extruded_filament_above_plates"].toDouble()),
		  z_coordinate_extruded_filament_inside_source_plate_(obj["z_coordinate_extruded_filament_inside_source_plate"].toDouble()),
		  z_coordinate_extruded_filament_inside_master_plate_(obj["z_coordinate_extruded_filament_inside_master_plate"].toDouble()),
		  z_coordinate_extruded_filament_inside_goal_plate_(obj["z_coordinate_extruded_filament_inside_goal_plate"].toDouble())
	{

	}

	PrinterProfile::PrinterProfile(
	const Point& cut_filament_position, float filament_extrusion_length,
	float z_coordinate_extruded_filament_above_plates,
	float z_coordinate_extruded_filament_inside_source_plate,
	float z_coordinate_extruded_filament_inside_master_plate,
	float z_coordinate_extruded_filament_inside_goal_plate)
	: JsonConstructable(QJsonObject()),
	  cut_filament_position_(cut_filament_position),
	  filament_extrusion_length_(filament_extrusion_length),
	  z_coordinate_extruded_filament_above_plates_(
		  z_coordinate_extruded_filament_above_plates),
	  z_coordinate_extruded_filament_inside_source_plate_(
		  z_coordinate_extruded_filament_inside_source_plate),
	  z_coordinate_extruded_filament_inside_master_plate_(
		  z_coordinate_extruded_filament_inside_master_plate),
	  z_coordinate_extruded_filament_inside_goal_plate_(
		  z_coordinate_extruded_filament_inside_goal_plate) {}

Point PrinterProfile::cutFilamentPosition() const {
  return cut_filament_position_;
}

float PrinterProfile::filamentExtrusionLength() const {
  return filament_extrusion_length_;
}

float PrinterProfile::zCoordinateFilamentAbovePlates() const {
  return z_coordinate_extruded_filament_above_plates_;
}

float PrinterProfile::zCoordinateFilamentInsideSourcePlate() const {
  return z_coordinate_extruded_filament_inside_source_plate_;
}

float PrinterProfile::zCoordinateFilamentInsideMasterPlate() const {
  return z_coordinate_extruded_filament_inside_master_plate_;
}

float PrinterProfile::zCoordinateFilamentInsideGoalPlate() const {
	return z_coordinate_extruded_filament_inside_goal_plate_;
}

void PrinterProfile::write(QJsonObject& obj) const
{
	obj["cut_filament_position_"] = (QJsonObject)cut_filament_position_;

	obj["filament_extrusion_length"] = filament_extrusion_length_;
	obj["z_coordinate_extruded_filament_above_plates"] = z_coordinate_extruded_filament_above_plates_;
	obj["z_coordinate_extruded_filament_inside_source_plate"] = z_coordinate_extruded_filament_inside_source_plate_;
	obj["z_coordinate_extruded_filament_inside_master_plate"] = z_coordinate_extruded_filament_inside_master_plate_;
	obj["z_coordinate_extruded_filament_inside_goal_plate"] = z_coordinate_extruded_filament_inside_goal_plate_;
}
}  // namespace c3picko
