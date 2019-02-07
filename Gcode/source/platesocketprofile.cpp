#include "include/platesocketprofile.h"

namespace c3picko {

PlateSocketProfile::PlateSocketProfile(const QJsonObject& obj)
    : JsonConstructable(obj),
      depth_of_cutout_the_goal_plate_lies_in_(
          obj["depth_of_cutout_the_goal_plate_lies_in"].toString().toDouble()),
      depth_of_cutout_the_master_plate_lies_in_(
          obj["depth_of_cutout_the_master_plate_lies_in"]
              .toString()
              .toDouble()),
      depth_of_cutout_the_source_plate_lies_in_(
          obj["depth_of_cutout_the_source_plate_lies_in"]
              .toString()
              .toDouble()),
      global_origin_of_goal_plate_(
          Point(obj["global_origin_of_goal_plate_x"].toString().toDouble(),
                obj["global_origin_of_goal_plate_y"].toString().toDouble())),
      global_origin_of_master_plate_(
          Point(obj["global_origin_of_master_plate_x"].toString().toDouble(),
                obj["global_origin_of_master_plate_y"].toString().toDouble())),
      global_origin_of_source_plate_(
          Point(obj["global_origin_of_source_plate_x"].toString().toDouble(),
                obj["global_origin_of_source_plate_y"].toString().toDouble())),
      orientation_of_goal_plate_(obj["orientation_of_goal_plate"].toString() ==
                                         "kFirstRowFirstColumnAtCutoutOrigin"
                                     ? kFirstRowFirstColumnAtCutoutOrigin
                                     : kLastRowFirstColumnAtCutoutOrigin),
      socket_origin_offset_x_(
          obj["socket_origin_offset_x"].toString().toDouble()),
      socket_origin_offset_y_(
          obj["socket_origin_offset_y"].toString().toDouble()),
      socket_origin_offset_z_(
          obj["socket_origin_offset_z"].toString().toDouble()) {}

PlateSocketProfile::PlateSocketProfile(
    const Point& global_origin_of_source_plate,
    const Point& global_origin_of_master_plate,
    const Point& global_origin_of_goal_plate,
    GoalPlateOrientation orientation_of_goal_plate,
    float depth_of_cutout_the_source_plate_lies_in,
    float depth_of_cutout_the_master_plate_lies_in,
    float depth_of_cutout_the_goal_plate_lies_in, float socket_origin_offset_x,
    float socket_origin_offset_y, float socket_origin_offset_z)
    : JsonConstructable(QJsonObject()),
      global_origin_of_source_plate_(global_origin_of_source_plate),
      global_origin_of_master_plate_(global_origin_of_master_plate),
      global_origin_of_goal_plate_(global_origin_of_goal_plate),
      orientation_of_goal_plate_(orientation_of_goal_plate),
      depth_of_cutout_the_source_plate_lies_in_(
          depth_of_cutout_the_source_plate_lies_in),
      depth_of_cutout_the_master_plate_lies_in_(
          depth_of_cutout_the_master_plate_lies_in),
      depth_of_cutout_the_goal_plate_lies_in_(
          depth_of_cutout_the_goal_plate_lies_in),
      socket_origin_offset_x_(socket_origin_offset_x),
      socket_origin_offset_y_(socket_origin_offset_y),
      socket_origin_offset_z_(socket_origin_offset_z) {}

float PlateSocketProfile::originOffsetX() const {
  return socket_origin_offset_x_;
}

float PlateSocketProfile::originOffsetY() const {
  return socket_origin_offset_y_;
}

float PlateSocketProfile::originOffsetZ() const {
  return socket_origin_offset_z_;
}

float PlateSocketProfile::depthOfCutoutSourcePlateLiesIn() const {
  return depth_of_cutout_the_source_plate_lies_in_;
}

float PlateSocketProfile::depthOfCutoutMasterPlateLiesIn() const {
  return depth_of_cutout_the_master_plate_lies_in_;
}

float PlateSocketProfile::depthOfCutoutGoalPlateLiesIn() const {
  return depth_of_cutout_the_goal_plate_lies_in_;
}

Point PlateSocketProfile::originOfSourcePlate() const {
  return global_origin_of_source_plate_;
}

Point PlateSocketProfile::originOfMasterPlate() const {
  return global_origin_of_master_plate_;
}

Point PlateSocketProfile::originOfGoalPlate() const {
  return global_origin_of_goal_plate_;
}

GoalPlateOrientation PlateSocketProfile::orientationOfGoalPlate() const {
  return orientation_of_goal_plate_;
}

void PlateSocketProfile::write(QJsonObject& obj) const {
  obj["depth_of_cutout_the_goal_plate_lies_in"] =
      QString::number(depth_of_cutout_the_goal_plate_lies_in_);
  obj["depth_of_cutout_the_master_plate_lies_in"] =
      QString::number(depth_of_cutout_the_master_plate_lies_in_);
  obj["depth_of_cutout_the_source_plate_lies_in"] =
      QString::number(depth_of_cutout_the_source_plate_lies_in_);
  obj["global_origin_of_goal_plate_x"] =
      QString::number(global_origin_of_goal_plate_.xCoordinate());
  obj["global_origin_of_goal_plate_y"] =
      QString::number(global_origin_of_goal_plate_.yCoordinate());
  obj["global_origin_of_master_plate_x"] =
      QString::number(global_origin_of_master_plate_.xCoordinate());
  obj["global_origin_of_master_plate_y"] =
      QString::number(global_origin_of_master_plate_.yCoordinate());
  obj["global_origin_of_source_plate_x"] =
      QString::number(global_origin_of_source_plate_.xCoordinate());
  obj["global_origin_of_source_plate_y"] =
      QString::number(global_origin_of_source_plate_.yCoordinate());
  obj["orientation_of_goal_plate"] =
      (orientation_of_goal_plate_ == kFirstRowFirstColumnAtCutoutOrigin
           ? "kFirstRowFirstColumnAtCutoutOrigin"
           : "kLastRowFirstColumnAtCutoutOrigin");
  obj["socket_origin_offset_x"] = QString::number(socket_origin_offset_x_);
  obj["socket_origin_offset_y"] = QString::number(socket_origin_offset_y_);
  obj["socket_origin_offset_z"] = QString::number(socket_origin_offset_z_);
}

}  // namespace c3picko
