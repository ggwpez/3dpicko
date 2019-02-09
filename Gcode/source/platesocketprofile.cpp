#include "include/platesocketprofile.h"

namespace c3picko {
PlateSocketProfile::PlateSocketProfile(
	const Point& global_origin_of_source_plate,
	const Point& global_origin_of_master_plate,
	const Point& global_origin_of_goal_plate,
	GoalPlateOrientation orientation_of_goal_plate,
	float depth_of_cutout_the_source_plate_lies_in,
	float depth_of_cutout_the_master_plate_lies_in,
	float depth_of_cutout_the_goal_plate_lies_in, float socket_origin_offset_x,
	float socket_origin_offset_y, float socket_origin_offset_z)
	: global_origin_of_source_plate_(global_origin_of_source_plate),
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
}  // namespace c3picko
