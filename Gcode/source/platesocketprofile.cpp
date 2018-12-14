#include "include/platesocketprofile.h"

namespace c3picko {

PlateSocketProfile::PlateSocketProfile(const Point& global_origin_of_source_plate,
                                       const Point& global_origin_of_master_plate,
                                       const Point& global_origin_of_goal_plate,
                                       GoalPlateOrientation orientation_of_goal_plate,
                                       float socket_origin_offset_x,
                                       float socket_origin_offset_y)
  : global_origin_of_source_plate_(global_origin_of_source_plate),
    global_origin_of_master_plate_(global_origin_of_master_plate),
    global_origin_of_goal_plate_(global_origin_of_goal_plate),
    orientation_of_goal_plate_(orientation_of_goal_plate),
    socket_origin_offset_x_(socket_origin_offset_x),
    socket_origin_offset_y_(socket_origin_offset_y) {

}

float PlateSocketProfile::originOffsetX() const {
  return socket_origin_offset_x_;
}

float PlateSocketProfile::originOffsetY() const {
  return socket_origin_offset_y_;
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
} // namespace c3pick
