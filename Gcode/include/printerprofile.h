#ifndef PRINTERPROFILE_H
#define PRINTERPROFILE_H

#include "include/point.h"

namespace c3picko {

/**
 * @file printerprofile.h
 * @author flopicko
 * @brief Class representing the needed characteristics of the
 * used 3D printer, nozzle and filament.
 *
 */
class PrinterProfile {
 public:
  explicit PrinterProfile(
      int movement_speed, const Point &cut_filament_position_above_trigger,
      float z_coordinate_pushing_the_trigger,
      float filament_extrusion_length_on_move_cut_to_pick,
      float filament_extrusion_length_on_move_pick_to_master,
      float filament_extrusion_length_on_move_master_to_goal,
      float filament_extrusion_length_on_move_goal_to_cut,
      float filament_extrusion_length_on_pick,
      float filament_extrusion_length_on_put_onto_master,
      float filament_extrusion_length_after_cut,
      float length_of_removed_filament,
      float
          safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move);

  int movementSpeed() const;
  Point cutFilamentPosition() const;
  float zCoordinatePushingTheTrigger() const;
  float lengthOfRemovedFilament() const;
  float filamentExtrusionLengthOnMoveCutToPick() const;
  float filamentExtrusionLengthOnMovePickToMaster() const;
  float filamentExtrusionLengthOnMoveMasterToGoal() const;
  float filamentExtrusionLengthOnMoveGoalToCut() const;
  float filamentExtrusionLengthOnPick() const;
  float filamentExtrusionLengthOnPutOntoMaster() const;
  float filamentExtrusionLengthAfterCut();
  float safetyDistanceBetweenTopSurfaceOfAllPlatesAndNozzleOnMove() const;

 private:
  /**
   * @brief movement_speed_ the speed the nozzle is moved with,
   * in mm/min
   */
  const int movement_speed_;

  /**
   * @brief cut_filament_position_above_trigger_ the xyz position, in the
   * coordinate system of the printer, the nozzle needs to move to in order to
   * be above the trigger of the scissor and directly above the center of the
   * space between the scissors blades
   */
  const Point cut_filament_position_above_trigger_;

  /**
   * @brief z_coordinate_pushing_the_trigger_ the z coordinate
   * the trigger of the scissor is definetly pushed
   */
  const float z_coordinate_pushing_the_trigger_;

  /**
   * @brief length_of_removed_filament_ the length of the
   * filament that is cut, that was removed from the
   * rest of the filament
   */
  const float length_of_removed_filament_;
  /**
   * @brief filament_extrusion_length_on_move_cut_to_pick_ the length up to
   * which the filament shall be extruded while moving the nozzle from the
   * cutting position to the colony pick position above the source plate, in
   * millimeter, 3mm sufficient (determined by manual testing)
   */
  const float filament_extrusion_length_on_move_cut_to_pick_;

  /**
   * @brief filament_extrusion_length_on_move_pick_to_master_ the length up to
   * which the filament shall be extruded while moving the nozzle from the
   * colony pick position position to position above the master plate, in
   * millimeter, 3mm sufficient (determined by manual testing)
   */
  const float filament_extrusion_length_on_move_pick_to_master_;

  /**
   * @brief filament_extrusion_length_on_move_master_to_goal_ the length up to
   * which the filament shall be extruded while moving the nozzle from the
   * master plate above the goal plate, in
   * millimeter, 3mm sufficient (determined by manual testing)
   */
  const float filament_extrusion_length_on_move_master_to_goal_;

  /**
   * @brief filament_extrusion_length_on_move_goal_to_cut_ the length up to
   * which the filament shall be extruded while moving the nozzle from the
   * goal plate to the cut position, in millimeter,
   * 3mm sufficient (determined by manual testing)
   */
  const float filament_extrusion_length_on_move_goal_to_cut_;

  /**
   * @brief filament_extrusion_length_on_pick_
   * the length up to which the filament shall be extruded when a colony
   * is picked, in millimeter, 3mm sufficient (determined by manual testing)
   */
  const float filament_extrusion_length_on_pick_;

  /**
   * @brief filament_extrusion_length_on_put_onto_master_
   * the length up to which the filament shall be extruded when a colony
   * is put onto the master plate, in millimeter, 3mm sufficient (determined by
   * manual testing)
   */
  const float filament_extrusion_length_on_put_onto_master_;

  /**
   * @brief filament_extrusion_length_after_cut_ the length of the
   * extruded filament after cutting it, this equals
   * the distance between the tip of the nozzle and the gap
   * between the scissors blades, where the filament is cut in between,
   * while the trigger is pushed fullily
   */
  const float filament_extrusion_length_after_cut_;

  /**
   * @brief
   * safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move_ the
   * distance between the extruded filament of the moving nozzle and the top
   * surface of every plate, guaranteeing that the tip of the filament does not
   * touch anything on move, in millimeter,
   * 3mm sufficient (determined by manual testing)
   */
  const float
      safety_distance_between_top_surface_of_all_plates_and_nozzle_on_move_;
};
}  // namespace c3picko
#endif  // PRINTERPROFILE_H
