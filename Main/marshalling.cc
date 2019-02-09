#include "include/marshalling.hpp"
#include "include/plateprofile.h"
#include "include/platesocketprofile.h"
#include "include/printerprofile.h"

namespace c3picko {
template <>
QJsonObject Marshalling::toJson(const PrinterProfile& value) {
  QJsonObject obj;

  obj["movement_speed"] = QString::number(value.movementSpeed());
  obj["cut_filament_position_above_trigger_x"] =
	  QString::number(value.cutFilamentPosition().xCoordinate());
  obj["cut_filament_position_above_trigger_y"] =
	  QString::number(value.cutFilamentPosition().yCoordinate());
  obj["cut_filament_position_above_trigger_z"] =
	  QString::number(value.cutFilamentPosition().zCoordinate());
  obj["z_coordinate_pushing_the_trigger"] =
	  QString::number(value.zCoordinatePushingTheTrigger());
  obj["distance_between_pushed_trigger_and_gap_between_scissors_blade"] =
	  QString::number(
		  value.distanceBetweenPushedTriggerAndGapBetweenScissorsBlade());
  obj["filament_extrusion_length_on_move_offset"] =
	  QString::number(value.filamentExtrusionLengthOnMoveOffset());
  obj["filament_extrusion_length_on_pick_and_put_onto_master_plate_offset"] =
	  QString::number(
		  value.filamentExtrusionLengthOnPickAndPutOntoMasterPlateOffset());

  return obj;
}

template <>
PrinterProfile Marshalling::fromJson(const QJsonObject& obj) {
  return PrinterProfile(
	  obj["movement_speed"].toString().toInt(),
	  Point(obj["cut_filament_position_above_trigger_x"].toString().toDouble(),
			obj["cut_filament_position_above_trigger_y"].toString().toDouble(),
			obj["cut_filament_position_above_trigger_z"].toString().toDouble()),
	  obj["z_coordinate_pushing_the_trigger"].toString().toDouble(),
	  obj["distance_between_pushed_trigger_and_gap_between_scissors_blade"]
		  .toString()
		  .toDouble(),
	  obj["filament_extrusion_length_on_move_offset"].toString().toDouble(),
	  obj["filament_extrusion_length_on_pick_and_put_onto_master_plate_offset"]
		  .toString()
		  .toDouble());
}

template <>
QJsonObject Marshalling::toJson(const PlateSocketProfile& value) {
  QJsonObject obj;

  obj["depth_of_cutout_the_goal_plate_lies_in"] =
	  QString::number(value.depthOfCutoutGoalPlateLiesIn());
  obj["depth_of_cutout_the_master_plate_lies_in"] =
	  QString::number(value.depthOfCutoutMasterPlateLiesIn());
  obj["depth_of_cutout_the_source_plate_lies_in"] =
	  QString::number(value.depthOfCutoutSourcePlateLiesIn());

  obj["global_origin_of_goal_plate_x"] =
	  QString::number(value.originOfGoalPlate().xCoordinate());
  obj["global_origin_of_goal_plate_y"] =
	  QString::number(value.originOfGoalPlate().yCoordinate());

  obj["global_origin_of_master_plate_x"] =
	  QString::number(value.originOfMasterPlate().xCoordinate());
  obj["global_origin_of_master_plate_y"] =
	  QString::number(value.originOfMasterPlate().yCoordinate());

  obj["global_origin_of_source_plate_x"] =
	  QString::number(value.originOfSourcePlate().xCoordinate());
  obj["global_origin_of_source_plate_y"] =
	  QString::number(value.originOfSourcePlate().yCoordinate());

  obj["orientation_of_goal_plate"] =
	  (value.orientationOfGoalPlate() == kFirstRowFirstColumnAtCutoutOrigin
		   ? "kFirstRowFirstColumnAtCutoutOrigin"
		   : "kLastRowFirstColumnAtCutoutOrigin");
  obj["socket_origin_offset_x"] = QString::number(value.originOffsetX());
  obj["socket_origin_offset_y"] = QString::number(value.originOffsetY());
  obj["socket_origin_offset_z"] = QString::number(value.originOffsetZ());

  return obj;
}

template <>
PlateSocketProfile Marshalling::fromJson(const QJsonObject& obj) {
  return PlateSocketProfile(
	  Point(obj["global_origin_of_source_plate_x"].toString().toDouble(),
			obj["global_origin_of_source_plate_y"].toString().toDouble()),
	  Point(obj["global_origin_of_master_plate_x"].toString().toDouble(),
			obj["global_origin_of_master_plate_y"].toString().toDouble()),
	  Point(obj["global_origin_of_goal_plate_x"].toString().toDouble(),
			obj["global_origin_of_goal_plate_y"].toString().toDouble()),
	  obj["orientation_of_goal_plate"].toString() ==
			  "kFirstRowFirstColumnAtCutoutOrigin"
		  ? kFirstRowFirstColumnAtCutoutOrigin
		  : kLastRowFirstColumnAtCutoutOrigin,
	  obj["depth_of_cutout_the_source_plate_lies_in"].toString().toDouble(),
	  obj["depth_of_cutout_the_master_plate_lies_in"].toString().toDouble(),

	  obj["depth_of_cutout_the_goal_plate_lies_in"].toString().toDouble(),

	  obj["socket_origin_offset_x"].toString().toDouble(),
	  obj["socket_origin_offset_y"].toString().toDouble(),

	  obj["socket_origin_offset_z"].toString().toDouble());
}

template <>
QJsonObject Marshalling::toJson(const PlateProfile& value) {
  QJsonObject obj;

  obj["a1_column_offset"] = QString::number(value.a1ColumnOffset());
  obj["a1_row_offset"] = QString::number(value.a1RowOffset());
  obj["culture_medium_thickness"] =
	  QString::number(value.cultureMediumThickness());
  obj["height_goal_plate"] = QString::number(value.heightGoalPlate());
  obj["height_master_plate"] = QString::number(value.heightMasterPlate());
  obj["height_source_plate"] = QString::number(value.heightSourcePlate());
  obj["number_of_columns"] = QString::number(value.numberOfColumns());
  obj["number_of_rows"] = QString::number(value.numberOfRows());
  obj["well_depth"] = QString::number(value.wellDepth());
  obj["well_spacing_center_to_center"] =
	  QString::number(value.wellSpacingCenterToCenter());

  return obj;
}

template <>
PlateProfile Marshalling::fromJson(const QJsonObject& obj) {
  return PlateProfile(
	  obj["number_of_rows"].toString().toInt(),
	  obj["number_of_columns"].toString().toInt(),
	  obj["a1_row_offset"].toString().toDouble(),
	  obj["a1_column_offset"].toString().toDouble(),
	  obj["well_spacing_center_to_center"].toString().toDouble(),
	  obj["height_source_plate"].toString().toDouble(),
	  obj["height_master_plate"].toString().toDouble(),
	  obj["height_goal_plate"].toString().toDouble(),
	  obj["well_depth"].toString().toDouble(),
	  obj["culture_medium_thickness"].toString().toDouble());
}

template <>
QJsonObject Marshalling::toJson(const Point& value) {
  QJsonObject obj;

  obj["x_coordinate"] = value.xCoordinate();
  obj["y_coordinate"] = value.yCoordinate();
  obj["z_coordinate"] = value.zCoordinate();

  return obj;
}

template <>
Point Marshalling::fromJson(const QJsonObject& obj) {
  return Point(obj["x_coordinate"].toString().toDouble(),
			   obj["y_coordinate"].toString().toDouble(),
			   obj["z_coordinate"].toString().toDouble());
}
}
