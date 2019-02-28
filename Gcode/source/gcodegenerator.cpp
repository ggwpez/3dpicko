#include "include/gcodegenerator.h"

#include <algorithm>

namespace c3picko {

GcodeGenerator::GcodeGenerator(const PlateSocketProfile &plate_socket_profile,
                               const PrinterProfile &printer_profile,
                               const PlateProfile &plate_profile)
    : plate_socket_profile_(plate_socket_profile),
      printer_profile_(printer_profile),
      plate_profile_(plate_profile),
      gcode_lower_filament_onto_colony_(CreateGcodeLowerFilamentOntoColony()),
      gcode_lower_filament_onto_master_(CreateGcodeLowerFilamentOntoMaster()),
      gcode_align_tip_of_nozzle_with_top_of_well_(
          CreateGcodeAlignTipOfNozzleWithTopOfWell()),
      gcode_extrude_filament_until_bottom_of_well_(
          CreateGcodeExtrudeFilamentUntilBottomOfWell()),
      gcode_raise_filament_above_source_plate_(
          CreateGcodeRaiseFilamentAboveSourcePlate()),
      gcode_raise_filament_above_master_plate_(
          CreateGcodeRaiseFilamentAboveMasterPlate()),
      gcode_raise_filament_above_goal_plate_(
          CreateGcodeRaiseFilamentAboveGoalPlate()),
      gcode_move_to_cut_filament_position_above_trigger_(
          CreateGcodeMoveToCutFilemantPositionAboveTrigger()),
      gcode_extrude_filament_to_cut_length_(
          CreateGcodeExtrudeFilamentToCutLength()),
      gcode_push_trigger_(CreateGcodePushTheTrigger()),
      gcode_extrusion_length_on_move_cut_to_pick_(
          CreateGcodeExtrusionLengthOnMoveCutToPick()),
      gcode_extrusion_length_on_move_pick_to_master_(
          CreateGcodeExtrusionLengthOnMovePickToMaster()),
      gcode_extrusion_length_on_move_master_to_goal_(
          CreateGcodeExtrusionLengthOnMoveMasterToGoal()),
      gcode_extrusion_length_on_move_goal_to_cut_(
          CreateGcodeExtrusionLengthOnMoveGoalToCut()),
      gcode_gauge_filament_extrusion_length_(
          CreateGcodeGaugeFilamentExtrusionLength()),
      global_well_xy_coordinates_(ComputeGlobalWellCoordinates()),
      global_master_xy_coordinates_(ComputeGlobalMasterCoordinates()) {}

std::vector<GcodeInstruction>
GcodeGenerator::CreateGcodeForTheEntirePickingProcess(
    int row, int column,
    std::vector<LocalColonyCoordinates> local_colony_coordinates) {
  int starting_well = ComputeStartingWell(row, column);
  std::vector<GcodeInstruction> gcodes = Init();
  gcodes.reserve(local_colony_coordinates.size());
  //  gcodes.push_back(gcode_raise_filament_above_source_plate_);
  gcodes.push_back(gcode_raise_filament_above_goal_plate_); // TODO
  gcodes.push_back(gcode_move_to_cut_filament_position_above_trigger_);
  gcodes.push_back(gcode_extrude_filament_to_cut_length_);
  gcodes.push_back(gcode_push_trigger_);
  gcodes.push_back(gcode_move_to_cut_filament_position_above_trigger_);
  gcodes.push_back(gcode_gauge_filament_extrusion_length_);

  for (size_t i = 0; i < local_colony_coordinates.size(); ++i) {
    const GlobalColonyCoordinates global_colony_coordinate =
        MapLocalColonyCoordinateToGlobal(local_colony_coordinates.at(i));
    const GlobalWellCoordinates &global_well_coordinate =
        global_well_xy_coordinates_.at(starting_well + i);
    const GlobalMasterCoordinates &global_master_coordinate =
        global_master_xy_coordinates_.at(starting_well + i);

    gcodes.push_back(gcode_extrusion_length_on_move_cut_to_pick_);
    gcodes.push_back(
        GcodeInstruction::MoveToXY(global_colony_coordinate.xCoordinate(),
                                   global_colony_coordinate.yCoordinate()));
    gcodes.push_back(gcode_lower_filament_onto_colony_);
    gcodes.push_back(gcode_raise_filament_above_source_plate_);

    gcodes.push_back(gcode_extrusion_length_on_move_pick_to_master_);
    gcodes.push_back(
        GcodeInstruction::MoveToXY(global_master_coordinate.xCoordinate(),
                                   global_master_coordinate.yCoordinate()));
    gcodes.push_back(gcode_lower_filament_onto_master_);
    gcodes.push_back(gcode_raise_filament_above_master_plate_);

    gcodes.push_back(gcode_extrusion_length_on_move_master_to_goal_);
    gcodes.push_back(
        GcodeInstruction::MoveToXY(global_well_coordinate.xCoordinate(),
                                   global_well_coordinate.yCoordinate()));
    gcodes.push_back(gcode_align_tip_of_nozzle_with_top_of_well_);
    gcodes.push_back(gcode_extrude_filament_until_bottom_of_well_);
    gcodes.push_back(gcode_extrusion_length_on_move_goal_to_cut_);
    gcodes.push_back(gcode_raise_filament_above_goal_plate_);

    gcodes.push_back(gcode_move_to_cut_filament_position_above_trigger_);
    gcodes.push_back(gcode_extrude_filament_to_cut_length_);
    gcodes.push_back(gcode_push_trigger_);
    gcodes.push_back(gcode_move_to_cut_filament_position_above_trigger_);
    gcodes.push_back(gcode_gauge_filament_extrusion_length_);
  }

  return gcodes;
}

std::vector<GcodeInstruction> GcodeGenerator::Init() {
  std::vector<GcodeInstruction> gcode_init;
  gcode_init.reserve(6);
  gcode_init.push_back(GcodeInstruction::SetUnit());
  gcode_init.push_back(GcodeInstruction::AbsolutePositioning());
  gcode_init.push_back(GcodeInstruction::ExtrusionModeAbsolute());
  gcode_init.push_back(GcodeInstruction::Home());
  gcode_init.push_back(GcodeInstruction::AllowColdExtrusion());
  gcode_init.push_back(
      GcodeInstruction::SetMovementSpeed(printer_profile_.movementSpeed()));

  return gcode_init;
}

GlobalColonyCoordinates GcodeGenerator::MapLocalColonyCoordinateToGlobal(
    LocalColonyCoordinates &local_colony) {
  return GlobalColonyCoordinates(
      local_colony.xCoordinate() +
          plate_socket_profile_.originOfSourcePlate().xCoordinate() +
          plate_socket_profile_.originOffsetX(),
      local_colony.yCoordinate() +
          plate_socket_profile_.originOfSourcePlate().yCoordinate() +
          plate_socket_profile_.originOffsetY());
}

GcodeInstruction GcodeGenerator::CreateGcodeLowerFilamentOntoColony() {
  return GcodeInstruction::MoveToZ(
      plate_socket_profile_.originOffsetZ() -
      plate_socket_profile_.depthOfCutoutSourcePlateLiesIn() +
      plate_profile_.cultureMediumThicknessSourcePlate() +
      printer_profile_.filamentExtrusionLengthOnPick() -
      1  // push a little into the culture medium
  );
}

GcodeInstruction GcodeGenerator::CreateGcodeLowerFilamentOntoMaster() {
  return GcodeInstruction::MoveToZ(
      plate_socket_profile_.originOffsetZ() -
      plate_socket_profile_.depthOfCutoutMasterPlateLiesIn() +
      plate_profile_.cultureMediumThicknessMasterPlate() +
      printer_profile_.filamentExtrusionLengthOnPutOntoMaster() -
      1  // push a little into the culture medium
  );
}

GcodeInstruction GcodeGenerator::CreateGcodeAlignTipOfNozzleWithTopOfWell() {
  return GcodeInstruction::MoveToZ(
      plate_socket_profile_.originOffsetZ() -
      plate_socket_profile_.depthOfCutoutGoalPlateLiesIn() +
      plate_profile_.heightGoalPlate());
}

GcodeInstruction GcodeGenerator::CreateGcodeExtrudeFilamentUntilBottomOfWell() {
  return GcodeInstruction::ExtrudeFilament(plate_profile_.wellDepth());
}

float GcodeGenerator::ComputeZCoordinateTipOfNozzleAlignedHighest() const {
  float z_coordinate_tip_of_nozzle_aligned_with_top_surface_of_source_plate =
      plate_socket_profile_.originOffsetZ() -
      plate_socket_profile_.depthOfCutoutSourcePlateLiesIn() +
      plate_profile_.heightSourcePlate();

  float z_coordinate_tip_of_nozzle_aligned_with_top_surface_of_master_plate =
      plate_socket_profile_.originOffsetZ() -
      plate_socket_profile_.depthOfCutoutMasterPlateLiesIn() +
      plate_profile_.heightMasterPlate();

  float z_coordinate_tip_of_nozzle_aligned_with_top_surface_of_goal_plate =
      plate_socket_profile_.originOffsetZ() -
      plate_socket_profile_.depthOfCutoutGoalPlateLiesIn() +
      plate_profile_.heightGoalPlate();

  float z_coordinate_tip_of_nozzle_aligned_highest = std::max(
      std::max(
          z_coordinate_tip_of_nozzle_aligned_with_top_surface_of_source_plate,
          z_coordinate_tip_of_nozzle_aligned_with_top_surface_of_master_plate),
      z_coordinate_tip_of_nozzle_aligned_with_top_surface_of_goal_plate);

  return z_coordinate_tip_of_nozzle_aligned_highest;
}

GcodeInstruction GcodeGenerator::CreateGcodeRaiseFilamentAboveSourcePlate() {
  return GcodeInstruction::MoveToZ(
      ComputeZCoordinateTipOfNozzleAlignedHighest() +
      printer_profile_.filamentExtrusionLengthOnMovePickToMaster() +
      printer_profile_
          .safetyDistanceBetweenTopSurfaceOfAllPlatesAndNozzleOnMove());
}

GcodeInstruction GcodeGenerator::CreateGcodeRaiseFilamentAboveMasterPlate() {
  return GcodeInstruction::MoveToZ(
      ComputeZCoordinateTipOfNozzleAlignedHighest() +
      printer_profile_.filamentExtrusionLengthOnMoveMasterToGoal() +
      printer_profile_
          .safetyDistanceBetweenTopSurfaceOfAllPlatesAndNozzleOnMove());
}

GcodeInstruction GcodeGenerator::CreateGcodeRaiseFilamentAboveGoalPlate() {
  return GcodeInstruction::MoveToZ(
      ComputeZCoordinateTipOfNozzleAlignedHighest() +
      printer_profile_.filamentExtrusionLengthOnMoveGoalToCut() +
      printer_profile_
          .safetyDistanceBetweenTopSurfaceOfAllPlatesAndNozzleOnMove());
}

GcodeInstruction
GcodeGenerator::CreateGcodeMoveToCutFilemantPositionAboveTrigger() {
  return GcodeInstruction::MoveToXYZ(
      printer_profile_.cutFilamentPosition().xCoordinate(),
      printer_profile_.cutFilamentPosition().yCoordinate(),
      printer_profile_.cutFilamentPosition().zCoordinate());
}

GcodeInstruction GcodeGenerator::CreateGcodePushTheTrigger() {
  return GcodeInstruction::MoveToZ(
      printer_profile_.zCoordinatePushingTheTrigger());
}

GcodeInstruction GcodeGenerator::CreateGcodeExtrudeFilamentToCutLength() {
  return GcodeInstruction::ExtrudeFilament(
      printer_profile_.filamentExtrusionLengthAfterCut() +
      printer_profile_
          .lengthOfRemovedFilament());  // length of filament's tip are cut
}

GcodeInstruction GcodeGenerator::CreateGcodeExtrusionLengthOnMoveCutToPick() {
  return GcodeInstruction::ExtrudeFilament(
      printer_profile_.filamentExtrusionLengthOnMoveCutToPick());
}

GcodeInstruction
GcodeGenerator::CreateGcodeExtrusionLengthOnMovePickToMaster() {
  return GcodeInstruction::ExtrudeFilament(
      printer_profile_.filamentExtrusionLengthOnMovePickToMaster());
}

GcodeInstruction
GcodeGenerator::CreateGcodeExtrusionLengthOnMoveMasterToGoal() {
  return GcodeInstruction::ExtrudeFilament(
      printer_profile_.filamentExtrusionLengthOnMoveMasterToGoal());
}

GcodeInstruction GcodeGenerator::CreateGcodeExtrusionLengthOnMoveGoalToCut() {
  return GcodeInstruction::ExtrudeFilament(
      printer_profile_.filamentExtrusionLengthOnMoveGoalToCut());
}

GcodeInstruction GcodeGenerator::CreateGcodeGaugeFilamentExtrusionLength() {
  return GcodeInstruction::GaugeFilamentExtrusionLength(
      printer_profile_.filamentExtrusionLengthAfterCut());
}

std::vector<GlobalWellCoordinates>
GcodeGenerator::ComputeGlobalWellCoordinates() {
  return ComputeGlobalWellAndMasterCoordinates(
      plate_socket_profile_.originOfGoalPlate());
}

std::vector<GlobalMasterCoordinates>
GcodeGenerator::ComputeGlobalMasterCoordinates() {
  return ComputeGlobalWellAndMasterCoordinates(
      plate_socket_profile_.originOfMasterPlate());
}

std::vector<Point> GcodeGenerator::ComputeGlobalWellAndMasterCoordinates(
    const Point &origin_of_plate) {
  std::vector<GlobalWellCoordinates> global_well_coordinates;
  global_well_coordinates.reserve(plate_profile_.numberOfWells());

  switch (plate_socket_profile_.orientationOfGoalPlate()) {
    case kFirstRowFirstColumnAtCutoutOrigin:
      ComputeGlobalCoordinatesFirstRowFirstColumnOrientation(
          global_well_coordinates, origin_of_plate);
      break;
    case kLastRowFirstColumnAtCutoutOrigin:
      ComputeGlobalCoordinatesLastRowFirstColumnOrientation(
          global_well_coordinates, origin_of_plate);
      break;
  }

  return global_well_coordinates;
}

void GcodeGenerator::ComputeGlobalCoordinatesFirstRowFirstColumnOrientation(
    std::vector<Point> &global_well_coordintes, const Point &origin_of_plate) {
  for (int i = 0; i < plate_profile_.numberOfColumns(); ++i) {
    for (int j = 0; j < plate_profile_.numberOfRows(); ++j) {
      global_well_coordintes.push_back(
          Point(plate_profile_.a1RowOffset() +
                    j * plate_profile_.wellSpacingCenterToCenter() +
                    origin_of_plate.xCoordinate() +
                    plate_socket_profile_.originOffsetX(),
                plate_profile_.a1ColumnOffset() +
                    i * plate_profile_.wellSpacingCenterToCenter() +
                    origin_of_plate.yCoordinate() +
                    plate_socket_profile_.originOffsetY()));
    }
  }
}

void GcodeGenerator::ComputeGlobalCoordinatesLastRowFirstColumnOrientation(
    std::vector<Point> &global_well_coordintes, const Point &origin_of_plate) {
  for (int i = 0; i < plate_profile_.numberOfRows(); ++i) {
    for (int j = plate_profile_.numberOfColumns() - 1; j >= 0; --j) {
      global_well_coordintes.push_back(
          Point(plate_profile_.a1ColumnOffset() +
                    i * plate_profile_.wellSpacingCenterToCenter() +
                    origin_of_plate.xCoordinate() +
                    plate_socket_profile_.originOffsetX(),
                plate_profile_.a1RowOffset() +
                    j * plate_profile_.wellSpacingCenterToCenter() +
                    origin_of_plate.yCoordinate() +
                    plate_socket_profile_.originOffsetY()));
    }
  }
}

int GcodeGenerator::ComputeStartingWell(int row, int column) const {
  return (column - 1) * plate_profile_.numberOfRows() + row -
         1;  // -1 to accomodate to vector iterating starting from 0
}
}  // namespace c3picko
