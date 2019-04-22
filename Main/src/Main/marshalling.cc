#include "Main/marshalling.h"
#include <QJsonArray>
#include "PiCommunicator/octoconfig.h"
#include "Gcode/plateprofile.h"
#include "Gcode/platesocketprofile.h"
#include "Gcode/printerprofile.h"
#include "GUI/types/profile.h"

namespace c3picko {
template <>
QJsonObject Marshalling::toJson(const PrinterProfile& value) {
  QJsonObject obj;

  obj["movement_speed"] = (value.movementSpeed());
  obj["cut_filament_position_above_trigger"] =
	  Marshalling::toJson(value.cutFilamentPosition());
  obj["z_coordinate_pushing_the_trigger"] =
	  (value.zCoordinatePushingTheTrigger());
  obj["filament_extrusion_length_on_move_cut_to_pick"] =
	  value.filamentExtrusionLengthOnMoveCutToPick();
  obj["filament_extrusion_length_on_move_pick_to_master"] =
	  value.filamentExtrusionLengthOnMovePickToMaster();
  obj["filament_extrusion_length_on_move_master_to_goal"] =
	  value.filamentExtrusionLengthOnMoveMasterToGoal();
  obj["filament_extrusion_length_on_move_goal_to_cut"] =
	  value.filamentExtrusionLengthOnMoveGoalToCut();
  obj["filament_extrusion_length_on_pick"] =
	  value.filamentExtrusionLengthOnPick();
  obj["filament_extrusion_length_on_put_onto_master"] =
	  value.filamentExtrusionLengthOnPutOntoMaster();
  obj["filament_extrusion_length_after_cut"] =
	  value.filamentExtrusionLengthAfterCut();
  obj["length_of_removed_filament"] = value.lengthOfRemovedFilament();
  obj["safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move"] =
	  value.safetyDistanceBetweenTopSurfaceOfAllPlatesAndNozzleOnMove();

  return obj;
}

template <>
PrinterProfile Marshalling::fromJson(const QJsonObject& obj) {
  return PrinterProfile(
	  obj["movement_speed"].toInt(),
	  Marshalling::fromJson<Point>(
		  obj["cut_filament_position_above_trigger"].toObject()),
	  obj["z_coordinate_pushing_the_trigger"].toDouble(),
	  obj["filament_extrusion_length_on_move_cut_to_pick"].toDouble(),
	  obj["filament_extrusion_length_on_move_pick_to_master"].toDouble(),
	  obj["filament_extrusion_length_on_move_master_to_goal"].toDouble(),
	  obj["filament_extrusion_length_on_move_goal_to_cut"].toDouble(),
	  obj["filament_extrusion_length_on_pick"].toDouble(),
	  obj["filament_extrusion_length_on_put_onto_master"].toDouble(),
	  obj["filament_extrusion_length_after_cut"].toDouble(),
	  obj["length_of_removed_filament"].toDouble(),
	  obj["safety_distance_between_top_surface_of_all_plates_and_nozzle_on_"
		  "move"]
		  .toDouble());
}

template <>
QJsonObject Marshalling::toJson(const PlateSocketProfile& value) {
  QJsonObject obj;

  obj["global_origin_of_source_plate"] =
	  Marshalling::toJson(value.originOfSourcePlate());
  obj["global_origin_of_master_plate"] =
	  Marshalling::toJson(value.originOfMasterPlate());
  obj["global_origin_of_goal_plate"] =
	  Marshalling::toJson(value.originOfGoalPlate());

  obj["orientation_of_goal_plate"] =
	  (value.orientationOfGoalPlate() == kFirstRowFirstColumnAtCutoutOrigin
		   ? "kFirstRowFirstColumnAtCutoutOrigin"
		   : "kLastRowFirstColumnAtCutoutOrigin");
  obj["depth_of_cutout_the_goal_plate_lies_in"] =
	  (value.depthOfCutoutGoalPlateLiesIn());
  obj["depth_of_cutout_the_master_plate_lies_in"] =
	  (value.depthOfCutoutMasterPlateLiesIn());
  obj["depth_of_cutout_the_source_plate_lies_in"] =
	  (value.depthOfCutoutSourcePlateLiesIn());

  obj["socket_origin_offset"] =
	  Marshalling::toJson(Point(value.originOffsetX(), value.originOffsetY(),
								value.originOffsetZ()));  // TODO Dumb

  return obj;
}

template <>
PlateSocketProfile Marshalling::fromJson(const QJsonObject& obj) {
  Point socket_origin_offset(
	  Marshalling::fromJson<Point>(obj["socket_origin_offset"].toObject()));

  return PlateSocketProfile(
	  Marshalling::fromJson<Point>(
		  obj["global_origin_of_source_plate"].toObject()),
	  Marshalling::fromJson<Point>(
		  obj["global_origin_of_master_plate"].toObject()),
	  Marshalling::fromJson<Point>(
		  obj["global_origin_of_goal_plate"].toObject()),
	  Marshalling::fromJson<QString>(obj["orientation_of_goal_plate"]) ==
			  "kFirstRowFirstColumnAtCutoutOrigin"
		  ? kFirstRowFirstColumnAtCutoutOrigin
		  : kLastRowFirstColumnAtCutoutOrigin,
	  obj["depth_of_cutout_the_source_plate_lies_in"].toDouble(),
	  obj["depth_of_cutout_the_master_plate_lies_in"].toDouble(),
	  obj["depth_of_cutout_the_goal_plate_lies_in"].toDouble(),
	  socket_origin_offset.xCoordinate(), socket_origin_offset.yCoordinate(),
	  socket_origin_offset.zCoordinate());
}

template <>
QJsonObject Marshalling::toJson(const PlateProfile& value) {
  QJsonObject obj;

  obj["number_of_rows"] = value.numberOfRows();
  obj["number_of_columns"] = value.numberOfColumns();
  obj["a1_row_offset"] = value.a1RowOffset();
  obj["a1_column_offset"] = value.a1ColumnOffset();
  obj["well_spacing_center_to_center"] = value.wellSpacingCenterToCenter();
  obj["height_source_plate"] = value.heightSourcePlate();
  obj["height_master_plate"] = value.heightMasterPlate();
  obj["height_goal_plate"] = value.heightGoalPlate();
  obj["well_depth"] = value.wellDepth();
  obj["culture_medium_thickness_source_plate"] =
	  value.cultureMediumThicknessSourcePlate();
  obj["culture_medium_thickness_master_plate"] =
	  value.cultureMediumThicknessMasterPlate();

  return obj;
}

template <>
PlateProfile Marshalling::fromJson(const QJsonObject& obj) {
  return PlateProfile(
	  obj["number_of_rows"].toInt(), obj["number_of_columns"].toInt(),
	  obj["a1_row_offset"].toDouble(), obj["a1_column_offset"].toDouble(),
	  obj["well_spacing_center_to_center"].toDouble(),
	  obj["height_source_plate"].toDouble(),
	  obj["height_master_plate"].toDouble(),
	  obj["height_goal_plate"].toDouble(), obj["well_depth"].toDouble(),
	  obj["culture_medium_thickness_source_plate"].toDouble(),
	  obj["culture_medium_thickness_master_plate"].toDouble());
}

template <>
QJsonObject Marshalling::toJson(const Point& value) {
  QJsonObject obj;

  obj["x"] = value.xCoordinate();
  obj["y"] = value.yCoordinate();
  obj["z"] = value.zCoordinate();

  return obj;
}

template <>
Point Marshalling::fromJson(const QJsonObject& obj) {
  return Point(obj["x"].toDouble(), obj["y"].toDouble(), obj["z"].toDouble());
}

template <>
QDateTime Marshalling::fromJson(const QJsonObject& obj) {
  return QDateTime::fromMSecsSinceEpoch(obj["ms"].toVariant().toLongLong());
}

template <>
QJsonObject Marshalling::toJson(const QDateTime& value) {
  QJsonObject obj;

  obj["ms"] = value.toMSecsSinceEpoch();
  obj["formatted"] = value.toString(dateTimeFormat());

  return obj;
}

template <>
QSet<qint32> Marshalling::fromJson(const QJsonObject& obj) {
  QJsonArray elements = obj["elements"].toArray();
  QSet<qint32> ret;

  for (QJsonValueRef element : elements) ret.insert(element.toInt());

  return ret;
}

template <>
QJsonObject Marshalling::toJson(const QSet<qint32>& value) {
  QJsonObject obj;
  QJsonArray elements;

  for (auto it = value.begin(); it != value.end(); ++it)
	elements.push_back(*it);

  obj["elements"] = elements;
  return obj;
}

template <>
QSet<QString> Marshalling::fromJson(const QJsonObject& obj) {
  QJsonArray elements = obj["elements"].toArray();
  QSet<QString> ret;

  for (QJsonValueRef element : elements)
	ret.insert(Marshalling::fromJson<QString>(element));

  return ret;
}

template <>
QJsonObject Marshalling::toJson(const QSet<QString>& value) {
  QJsonObject obj;
  QJsonArray elements;

  for (auto it = value.begin(); it != value.end(); ++it)
	elements.push_back(*it);

  obj["elements"] = elements;
  return obj;
}
}  // namespace c3picko
