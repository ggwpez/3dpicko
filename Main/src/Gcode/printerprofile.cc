#include "Gcode/printerprofile.h"

namespace c3picko {

PrinterProfile::PrinterProfile(
    int movement_speed, const Point &cut_filament_position_above_trigger,
    float z_coordinate_pushing_the_trigger,
    float filament_extrusion_length_on_move_cut_to_pick,
    float filament_extrusion_length_on_move_pick_to_master,
    float filament_extrusion_length_on_move_master_to_goal,
    float filament_extrusion_length_on_move_goal_to_cut,
    float filament_extrusion_length_on_pick,
    float filament_extrusion_length_on_put_onto_master,
    float filament_extrusion_length_after_cut, float length_of_removed_filament,
    float safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move)
    : movement_speed_(movement_speed),
      cut_filament_position_above_trigger_(cut_filament_position_above_trigger),
      z_coordinate_pushing_the_trigger_(z_coordinate_pushing_the_trigger),
      length_of_removed_filament_(length_of_removed_filament),
      filament_extrusion_length_on_move_cut_to_pick_(
          filament_extrusion_length_on_move_cut_to_pick),
      filament_extrusion_length_on_move_pick_to_master_(
          filament_extrusion_length_on_move_pick_to_master),
      filament_extrusion_length_on_move_master_to_goal_(
          filament_extrusion_length_on_move_master_to_goal),
      filament_extrusion_length_on_move_goal_to_cut_(
          filament_extrusion_length_on_move_goal_to_cut),
      filament_extrusion_length_on_pick_(filament_extrusion_length_on_pick),
      filament_extrusion_length_on_put_onto_master_(
          filament_extrusion_length_on_put_onto_master),
      filament_extrusion_length_after_cut_(filament_extrusion_length_after_cut),
      safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move_(
          safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move) {
}

int PrinterProfile::movementSpeed() const { return movement_speed_; }

Point PrinterProfile::cutFilamentPosition() const {
  return cut_filament_position_above_trigger_;
}

float PrinterProfile::zCoordinatePushingTheTrigger() const {
  return z_coordinate_pushing_the_trigger_;
}

float PrinterProfile::lengthOfRemovedFilament() const {
  return length_of_removed_filament_;
}

float PrinterProfile::filamentExtrusionLengthOnMoveCutToPick() const {
  return filament_extrusion_length_on_move_cut_to_pick_;
}

float PrinterProfile::filamentExtrusionLengthOnMovePickToMaster() const {
  return filament_extrusion_length_on_move_pick_to_master_;
}

float PrinterProfile::filamentExtrusionLengthOnMoveMasterToGoal() const {
  return filament_extrusion_length_on_move_master_to_goal_;
}

float PrinterProfile::filamentExtrusionLengthOnMoveGoalToCut() const {
  return filament_extrusion_length_on_move_goal_to_cut_;
}

float PrinterProfile::filamentExtrusionLengthOnPick() const {
  return filament_extrusion_length_on_pick_;
}

float PrinterProfile::filamentExtrusionLengthOnPutOntoMaster() const {
  return filament_extrusion_length_on_put_onto_master_;
}

float PrinterProfile::filamentExtrusionLengthAfterCut() const {
  return filament_extrusion_length_after_cut_;
}

float PrinterProfile::
    safetyDistanceBetweenTopSurfaceOfAllPlatesAndNozzleOnMove() const {
  return safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move_;
}

}  // namespace c3picko
