#include "include/marshalling.hpp"
#include "include/plateprofile.h"
#include "include/platesocketprofile.h"
#include "include/printerprofile.h"

namespace c3picko
{
template <> QJsonObject Marshalling::toJson(const PrinterProfile& value)
{
	QJsonObject obj;

	obj["movement_speed"]					   = (value.movementSpeed());
	obj["cut_filament_position_above_trigger"] = Marshalling::toJson(value.cutFilamentPosition());
	obj["z_coordinate_pushing_the_trigger"]	= (value.zCoordinatePushingTheTrigger());
	obj["distance_between_pushed_trigger_and_gap_between_scissors_blade"]
		= (value.distanceBetweenPushedTriggerAndGapBetweenScissorsBlade());
	obj["filament_extrusion_length_on_move_offset"] = (value.filamentExtrusionLengthOnMoveOffset());
	obj["filament_extrusion_length_on_pick_and_put_onto_master_plate_offset"]
		= (value.filamentExtrusionLengthOnPickAndPutOntoMasterPlateOffset());

	return obj;
}

template <> PrinterProfile Marshalling::fromJson(const QJsonObject& obj)
{
	return PrinterProfile(obj["movement_speed"].toInt(),
						  Marshalling::fromJson<Point>(obj["cut_filament_position_above_trigger"].toObject()),
						  obj["z_coordinate_pushing_the_trigger"].toDouble(),
						  obj["distance_between_pushed_trigger_and_gap_between_scissors_blade"].toDouble(),
						  obj["filament_extrusion_length_on_move_offset"].toDouble(),
						  obj["filament_extrusion_length_on_pick_and_put_onto_master_plate_offset"].toDouble());
}

template <> QJsonObject Marshalling::toJson(const PlateSocketProfile& value)
{
	QJsonObject obj;

	obj["depth_of_cutout_the_goal_plate_lies_in"]   = (value.depthOfCutoutGoalPlateLiesIn());
	obj["depth_of_cutout_the_master_plate_lies_in"] = (value.depthOfCutoutMasterPlateLiesIn());
	obj["depth_of_cutout_the_source_plate_lies_in"] = (value.depthOfCutoutSourcePlateLiesIn());

	obj["global_origin_of_source_plate"] = Marshalling::toJson(value.originOfSourcePlate());
	obj["global_origin_of_master_plate"] = Marshalling::toJson(value.originOfMasterPlate());
	obj["global_origin_of_goal_plate"]   = Marshalling::toJson(value.originOfGoalPlate());

	obj["orientation_of_goal_plate"]
		= (value.orientationOfGoalPlate() == kFirstRowFirstColumnAtCutoutOrigin ? "kFirstRowFirstColumnAtCutoutOrigin"
																				: "kLastRowFirstColumnAtCutoutOrigin");

	obj["socket_origin_offset"] = Marshalling::toJson(Point(value.originOffsetX(), value.originOffsetY(),
															value.originOffsetZ())); // TODO Dumb

	return obj;
}

template <> PlateSocketProfile Marshalling::fromJson(const QJsonObject& obj)
{
	Point socket_origin_offset(Marshalling::fromJson<Point>(obj["socket_origin_offset"].toObject()));

	return PlateSocketProfile(
		Marshalling::fromJson<Point>(obj["global_origin_of_source_plate"].toObject()),
		Marshalling::fromJson<Point>(obj["global_origin_of_master_plate"].toObject()),
		Marshalling::fromJson<Point>(obj["global_origin_of_goal_plate"].toObject()),
		obj["orientation_of_goal_plate"].toString() == "kFirstRowFirstColumnAtCutoutOrigin" ? kFirstRowFirstColumnAtCutoutOrigin
																							: kLastRowFirstColumnAtCutoutOrigin,
		obj["depth_of_cutout_the_source_plate_lies_in"].toDouble(), obj["depth_of_cutout_the_master_plate_lies_in"].toDouble(),
		obj["depth_of_cutout_the_goal_plate_lies_in"].toDouble(), socket_origin_offset.xCoordinate(), socket_origin_offset.yCoordinate(),
		socket_origin_offset.zCoordinate());
}

template <> QJsonObject Marshalling::toJson(const PlateProfile& value)
{
	QJsonObject obj;

	obj["a1_column_offset"]				 = (value.a1ColumnOffset());
	obj["a1_row_offset"]				 = (value.a1RowOffset());
	obj["culture_medium_thickness"]		 = (value.cultureMediumThickness());
	obj["height_goal_plate"]			 = (value.heightGoalPlate());
	obj["height_master_plate"]			 = (value.heightMasterPlate());
	obj["height_source_plate"]			 = (value.heightSourcePlate());
	obj["number_of_columns"]			 = (value.numberOfColumns());
	obj["number_of_rows"]				 = (value.numberOfRows());
	obj["well_depth"]					 = (value.wellDepth());
	obj["well_spacing_center_to_center"] = (value.wellSpacingCenterToCenter());

	return obj;
}

template <> PlateProfile Marshalling::fromJson(const QJsonObject& obj)
{
	return PlateProfile(obj["number_of_rows"].toInt(), obj["number_of_columns"].toInt(), obj["a1_row_offset"].toDouble(),
						obj["a1_column_offset"].toDouble(), obj["well_spacing_center_to_center"].toDouble(),
						obj["height_source_plate"].toDouble(), obj["height_master_plate"].toDouble(), obj["height_goal_plate"].toDouble(),
						obj["well_depth"].toDouble(), obj["culture_medium_thickness"].toDouble());
}

template <> QJsonObject Marshalling::toJson(const Point& value)
{
	QJsonObject obj;

	obj["x"] = value.xCoordinate();
	obj["y"] = value.yCoordinate();
	obj["z"] = value.zCoordinate();

	return obj;
}

template <> Point Marshalling::fromJson(const QJsonObject& obj)
{
	return Point(obj["x"].toDouble(), obj["y"].toDouble(), obj["z"].toDouble());
}
} // namespace c3picko
