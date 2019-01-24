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
	  const Point& cut_filament_position, float filament_extrusion_length,
	  float z_coordinate_extruded_filament_above_plates,
	  float z_coordinate_extruded_filament_inside_source_plate,
	  float z_coordinate_extruded_filament_inside_master_plate,
	  float z_coordinate_extruded_filament_inside_goal_plate);

  Point cutFilamentPosition() const;
  float filamentExtrusionLength() const;
  float zCoordinateFilamentAbovePlates() const;
  float zCoordinateFilamentInsideSourcePlate() const;
  float zCoordinateFilamentInsideMasterPlate() const;
  float zCoordinateFilamentInsideGoalPlate() const;

 private:
  /**
   * @brief cut_filament_position_ the xy position, in the coordinate system
   * of the printer, the nozzle needs to move to in order to have the filament
   * cut
   */
  const Point cut_filament_position_;

  /**
   * @brief filament_extrusion_length_ the length up to which the filament
   * shall be extruded, in millimeter
   */
  const float filament_extrusion_length_;

  /**
   * @brief z_coordinate_extruded_filament_above_plates_ the z coordinate, in
   * the coordinate system of the printer, the extruded filament can be moved
   * above the plates without touching them or anything else, in millimeter
   */
  const float z_coordinate_extruded_filament_above_plates_;

  /**
   * @brief z_coordinate_filament_inside_source_plate_ the z coordinate, in the
   * coordinate system of the printer, the extruded filament touches a colony
   * on the source plate, in millimeter
   */
  const float z_coordinate_extruded_filament_inside_source_plate_;

  /**
   * @brief z_coordinate_extruded_filament_inside_master_plate_ the z
   * coordinate, in the coordinate system of the printer, the extruded filament
   * touches the culture medium on the master plate, in millimeter
   */
  const float z_coordinate_extruded_filament_inside_master_plate_;

  /**
   * @brief z_coordinate_extruded_filament_inside_goal_plate_ the z coordinate,
   * in the coordinate system of the printer, the extruded filament touches
   * the liqour inside the well of the goal plate, in millimeter
   */
  const float z_coordinate_extruded_filament_inside_goal_plate_;
};
}  // namespace c3picko
#endif  // PRINTERPROFILE_H
