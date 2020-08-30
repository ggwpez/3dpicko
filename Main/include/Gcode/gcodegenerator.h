/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 flopicko flobotic@protonmail.com
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef GCODEGENERATOR_H
#define GCODEGENERATOR_H

#include "Gcode/gcodeinstruction.h"
#include "Gcode/plateprofile.h"
#include "Gcode/platesocketprofile.h"
#include "Gcode/point.h"
#include "Gcode/printerprofile.h"

namespace c3picko {

/**
 * @file gcodeinstruction.h
 * @author flopicko
 * @brief Class for creating the gcode neeeded for
 * an entire picking process. It takes care of the
 * mapping of local coordinates to global (socket) coordinates.
 *
 */
class GcodeGenerator {
 public:
  explicit GcodeGenerator(const PlateSocketProfile& plate_socket_profile,
						  const PrinterProfile& printer_profile,
						  const PlateProfile& master_and_goal_plate_profile);

  /**
   * @brief Creates the gcode for an entire picking process.
   * This process includes transfering colony by colony
   * to the master and the goal plate.
   * @param local_colony_coordinates The coordinates of the colonies
   * @param starting_row The row number of the well the first
   * colony should be transferred to.
   * @param starting_column The column number of the well the first
   * colony should be transferred to.
   * to be picked in their respective local cordinate system.
   * @return The gcode for the entire picking process.
   */
  std::vector<GcodeInstruction> CreateGcodeForTheEntirePickingProcess(
	  int starting_row, int starting_column,
	  std::vector<LocalColonyCoordinates> local_colony_coordinates);

 private:
  /**
   * @brief Creates a gcode instruction for initializing the printer.
   * This includes setting the positioning of nozzle and extrusion
   * to absolute and the homing of the nozzle.
   * @return a gcode instruction of the form " "
   */
  std::vector<GcodeInstruction> Init();

  /**
   * @brief MapLocalColonyCoordinateToGlobal transforms a local colony
   * coordinate into a global (socket) coordinate.
   * @param local_colony the local colony coordinate to be transformed into
   * a coordinate of the used socket's coordinate system
   * @return the colony coordinate in the socket's coordinate system
   */
  GlobalColonyCoordinates MapLocalColonyCoordinateToGlobal(
	  LocalColonyCoordinates& local_colony);

  GcodeInstruction CreateGcodeLowerFilamentOntoColony();
  GcodeInstruction CreateGcodeLowerFilamentOntoMaster();
  GcodeInstruction CreateGcodeAlignTipOfNozzleWithTopOfWell();
  GcodeInstruction CreateGcodeExtrudeFilamentUntilBottomOfWell();
  GcodeInstruction CreateGcodeRaiseFilamentAboveSourcePlate();
  GcodeInstruction CreateGcodeRaiseFilamentAboveMasterPlate();
  GcodeInstruction CreateGcodeRaiseFilamentAboveGoalPlate();
  /**
   * used for computing the gcode for
   * raising filaments above plate
   */
  float ComputeZCoordinateTipOfNozzleAlignedHighest() const;
  GcodeInstruction CreateGcodeMoveToCutFilemantPositionAboveTrigger();
  GcodeInstruction CreateGcodePushTheTrigger();
  GcodeInstruction CreateGcodeExtrudeFilamentToCutLength();
  GcodeInstruction CreateGcodeExtrusionLengthOnMoveCutToPick();
  GcodeInstruction CreateGcodeExtrusionLengthOnMovePickToMaster();
  GcodeInstruction CreateGcodeExtrusionLengthOnMoveMasterToGoal();
  GcodeInstruction CreateGcodeExtrusionLengthOnMoveGoalToCut();

  GcodeInstruction CreateGcodeGaugeFilamentExtrusionLength();

  /**
   * @brief See ComputeGlobalWellAndMasterCoordinates().
   */
  std::vector<GlobalWellCoordinates> ComputeGlobalWellCoordinates();

  /**
   * @brief See ComputeGlobalWellAndMasterCoordinates().
   */
  std::vector<GlobalMasterCoordinates> ComputeGlobalMasterCoordinates();

  /**
   * @brief Contains the real implementation for computing both the global
   * well xy coordinates and the global master xy coordinates.
   * ComputeGlobalWellCoordinates() and ComputeGlobalMasterCoordinates()
   * call this function providing the necessary arguments.
   */
  std::vector<Point> ComputeGlobalWellAndMasterCoordinates(
	  const Point& origin_of_plate);

  /**
   * @brief Compute the xy coordinates of the goal plate's wells represented in
   * the coordinate system of the socket, considering that the corner of the
   * goal plate where well A1 is at lies at the origin of the corresponding
   * cutout for the goal plate
   * @param global_well_coordintes The vector that is to be filled with the
   * global xy coordinates of the wells.
   */
  void ComputeGlobalCoordinatesFirstRowFirstColumnOrientation(
	  std::vector<Point>& global_well_coordintes, const Point& origin_of_plate);

  /**
   * @brief Compute the xy coordinates of the goal plate's wells represented in
   * the coordinate system of the socket, considering that the corner of the
   * goal plate where the well in the last row, first column is at lies at the
   * origin of the corresponding cutout for the goal plate
   * @param global_well_coordintes The vector that is to be filled with the
   * global xy coordinates of the wells.
   */
  void ComputeGlobalCoordinatesLastRowFirstColumnOrientation(
	  std::vector<Point>& global_well_coordintes, const Point& origin_of_plate);

  /**
   * @brief ComputeStartingWell computes the first well
   * the first colony is transferred to, considering columnwise
   * ordering.
   */
  int ComputeStartingWell(int row, int column) const;

  /**
   * @brief plate_socket_profile_ the currently used socket
   */
  PlateSocketProfile plate_socket_profile_;

  /**
   * @brief printer_profile_ the currently used printer
   */
  PrinterProfile printer_profile_;

  /**
   * @brief plate_profile_ the currently used
   * source, master and goal plate
   */
  PlateProfile plate_profile_;

  /**
   * @brief gcode_lower_filament_onto_colony_ the gcode instruction for lowering
   * the nozzle onto a colony on the current xy position.
   */
  GcodeInstruction gcode_lower_filament_onto_colony_;

  /**
   * @brief gcode_lower_filament_onto_master_ the gcode instruction for lowering
   * the nozzle onto the master plate touching the culture medium on the current
   * xy position.
   */
  GcodeInstruction gcode_lower_filament_onto_master_;

  /**
   * @brief gcode_align_tip_of_nozzle_with_top_of_well_ the gcode instruction
   * for aligning the tip of the nozzle with the top of the well it is
   * positioned above.
   */
  GcodeInstruction gcode_align_tip_of_nozzle_with_top_of_well_;

  /**
   * @brief gcode_extrude_filament_until_bottom_of_well_ the gcode instruction
   * for extruding the filament until its tip touches the bottom of the well
   */
  GcodeInstruction gcode_extrude_filament_until_bottom_of_well_;

  /**
   * @brief gcode_raise_filament_above_source_plate_ the gcode instruction for
   * raising the nozzle above source plate so that the extruded filament is not
   * touching the borders of the source plate while moving on the xy axes.
   */
  GcodeInstruction gcode_raise_filament_above_source_plate_;

  /**
   * @brief gcode_raise_filament_above_master_plate_ the gcode instruction for
   * raising the nozzle above source plate so that the extruded filament is not
   * touching the borders of the master plate while moving on the xy axes.
   */
  GcodeInstruction gcode_raise_filament_above_master_plate_;

  /**
   * @brief gcode_raise_filament_above_goal_plate_ the gcode instruction for
   * raising the nozzle above source plate so that the extruded filament is not
   * touching the borders of the goal plate while moving on the xy axes.
   */
  GcodeInstruction gcode_raise_filament_above_goal_plate_;

  /**
   * @brief gcode_move_to_cut_filament_position_above_trigger_ the gcode
   * instruction for moving the nozzle to the position where the filament is cut
   * but the trigger is not yet pushed.
   */
  GcodeInstruction gcode_move_to_cut_filament_position_above_trigger_;

  /**
   * @brief gcode_extrude_filament_to_cut_length_w the gcode instruction
   * for extruding the filament to a length so that the tip of it is
   * cut by pushing the trigger
   */
  GcodeInstruction gcode_extrude_filament_to_cut_length_;

  /**
   * @brief gcode_push_trigger_ the gcode instruction for
   * lowering the nozzle until the trigger is pushed fullily
   * so that the filament is cut
   */
  GcodeInstruction gcode_push_trigger_;

  /**
   * @brief gcode_extrusion_length_on_move_cut_to_pick_ the gcode instruction
   * for extruding the filament up to the length it shall have moving from
   * the cut position to the source plate.
   */
  GcodeInstruction gcode_extrusion_length_on_move_cut_to_pick_;

  /**
   * @brief gcode_extrusion_length_on_move_pick_to_master_ the gcode instruction
   * for extruding the filament up to the length it shall have moving from
   * source to master plate.
   */
  GcodeInstruction gcode_extrusion_length_on_move_pick_to_master_;

  /**
   * @brief gcode_extrusion_length_on_move_master_to_goal_ the gcode instruction
   * for extruding the filament up to the length it shall have moving from
   * the master to goal plate.
   */
  GcodeInstruction gcode_extrusion_length_on_move_master_to_goal_;

  /**
   * @brief gcode_extrusion_length_on_move_goal_to_cut_ the gcode instruction
   * for extruding the filament up to the length it shall have moving from
   * the goal plate to cut position.
   */
  GcodeInstruction gcode_extrusion_length_on_move_goal_to_cut_;

  /**
   * @brief gcode_gauge_filament_extrusion_length_ the gcode instruction
   * for gauging the length of the extruded filament
   */
  GcodeInstruction gcode_gauge_filament_extrusion_length_;

  /**
   * @brief global_well_xy_coordinates_ the xy coordinates
   *  of the wells of the goal plate, ordered columnwise,
   *  represented in the coordinate system of the used socket.
   */
  std::vector<GlobalWellCoordinates> global_well_xy_coordinates_;

  /**
   * @brief global_master_xy_coordinates_ the xy coordinates of the positions
   * the colonies are transferred to on the master plate, ordered columnwise
   * corresponding to the used goal plate, represented in the coordinate system
   * of the used socket.
   */
  std::vector<GlobalMasterCoordinates> global_master_xy_coordinates_;
};
}  // namespace c3picko
#endif	// GCODEGENERATOR_H
