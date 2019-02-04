#ifndef PRINTERPROFILE_H
#define PRINTERPROFILE_H

#include "include/point.h"

namespace c3picko {

/**
 * @file printerprofile.h
 * @author flopicko
 * @brief Class representing the needed characteristics of the
 * used 3D printer.
 *
 */
class PrinterProfile {
 public:
  explicit PrinterProfile(
      int movement_speed,
      const Point& cut_filament_position_above_trigger,
      float z_coordinate_pushing_the_trigger,
      float z_coordinate_distance_between_pushed_trigger_and_gap_between_scissors_blade,
      float filament_extrusion_length_on_move_offset = 0,
      float filament_extrusion_length_on_pick_and_put_onto_master_plate_offset = 0);

  int movementSpeed() const;
  Point cutFilamentPosition() const;
  float zCoordinatePushingTheTrigger() const;
  float distanceBetweenPushedTriggerAndGapBetweenScissorsBlade() const;
  float filamentExtrusionLengthOnMove() const;
  float filamentExtrusionLengthOnPickAndPutOntoMasterPlate() const;
  float safetyDistanceBetweenTopSurfaceOfAllPlatesAndNozzleOnMove() const;

 private:
  /**
   * @brief movement_speed_ the speed the nozzle is moved with,
   * in mm/min
   */
  const int movement_speed_;

  /**
   * @brief cut_filament_position_above_trigger_ the xyz position, in the coordinate system
   * of the printer, the nozzle needs to move to in order to be above the trigger of the
   * scissor and directly above the center of the space between the scissors blades
   */
  const Point cut_filament_position_above_trigger_;

  /**
   * @brief z_coordinate_pushing_the_trigger_ the z coordinate
   * the trigger of the scissor is definetly pushed
   */
  const float z_coordinate_pushing_the_trigger_;

  /**
   * @brief distance_between_pushed_trigger_and_gap_between_scissors_blade_
   * the distance between the trigger when it is pushed fullily and the gap
   * between the scissors blades, where the filament is cut in between
   */
  const float distance_between_pushed_trigger_and_gap_between_scissors_blade_;

  /**
   * @brief filament_extrusion_length_default_ the length up to which the filament
   * shall be extruded to at least, determined by manual testing, in millimeter
   */
  const float filament_extrusion_length_default_;

  /**
   * @brief filament_extrusion_length_on_move_ the length up to which the filament
   * shall be extruded while moving the nozzle above the plates, in millimeter
   */
  const float filament_extrusion_length_on_move_;

  /**
   * @brief filament_extrusion_length_on_pick_and_put_onto_master_plate_
   * the length up to which the filament shall be extruded when a colony
   * is picked and put onto the master plate, in millimeter
   */
  const float filament_extrusion_length_on_pick_and_put_onto_master_plate_;

  /**
   * @brief safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move_
   * the distance between the extruded filament of the moving nozzle and the top
   * surface of every plate, guaranteeing that the tip of the filament does not
   * touch anything on move, determined by manual testing, in millimeter
   */
  const float safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move_;
};
}  // namespace c3picko
#endif  // PRINTERPROFILE_H
