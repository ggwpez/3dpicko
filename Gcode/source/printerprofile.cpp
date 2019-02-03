#include "include/printerprofile.h"

namespace c3picko {

PrinterProfile::PrinterProfile(int movement_speed,
                               const Point& cut_filament_position_above_trigger,
                               float z_coordinate_pushing_the_trigger,
                               float z_coordinate_distance_between_pushed_trigger_and_gap_between_scissors_blade,
                               float filament_extrusion_length_on_move_offset,
                               float filament_extrusion_length_on_pick_and_put_onto_master_plate_offset)
  : movement_speed_(movement_speed),
    cut_filament_position_above_trigger_(cut_filament_position_above_trigger),
    z_coordinate_pushing_the_trigger_(z_coordinate_pushing_the_trigger),
    z_coordinate_distance_between_pushed_trigger_and_gap_between_scissors_blade_(z_coordinate_distance_between_pushed_trigger_and_gap_between_scissors_blade),
    filament_extrusion_length_default_(3),
    filament_extrusion_length_on_move_(filament_extrusion_length_default_ + filament_extrusion_length_on_move_offset),
    filament_extrusion_length_on_pick_and_put_onto_master_plate_(filament_extrusion_length_default_ + filament_extrusion_length_on_pick_and_put_onto_master_plate_offset),
    safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move_(2)
{}

int PrinterProfile::movementSpeed() const {
  return movement_speed_;
}

Point PrinterProfile::cutFilamentPosition() const {
  return cut_filament_position_above_trigger_;
}

float PrinterProfile::zCoordinatePushingTheTrigger() const {
  return z_coordinate_pushing_the_trigger_;
}

float PrinterProfile::zCoordinateDistanceBetweenPushedTriggerAndGapBetweenScissorsBlade() const {
  return z_coordinate_distance_between_pushed_trigger_and_gap_between_scissors_blade_;
}

float PrinterProfile::filamentExtrusionLengthOnMove() const {
  return filament_extrusion_length_on_move_;
}

float PrinterProfile::filamentExtrusionLengthOnPickAndPutOntoMasterPlate() const {
  return filament_extrusion_length_on_pick_and_put_onto_master_plate_;
}

float PrinterProfile::safetyDistanceBetweenTopSurfaceOfAllPlatesAndNozzleOnMove() const {
  return safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move_;
}

}  // namespace c3picko
